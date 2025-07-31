#include "mqttConnection.h"

char button_message[50] = "";
// err_t mqtt_test_connect(MQTT_CLIENT_T *state);

// Função de callback chamada quando a resolução de DNS terminar
void dns_found(const char *name, const ip_addr_t *ipaddr, void *callback_arg) {
    // cria uma variável do tipo MQTT_CLIENT_T a partir do argumento callback_arg
    MQTT_CLIENT_T *state = (MQTT_CLIENT_T*)callback_arg;
    
    if (ipaddr) {
        // Armazena o IP resolvido na estrutura
        state->remote_addr = *ipaddr;
        // Mostra IP resolvido no console
        printf("DNS resovido: %s\n", ip4addr_ntoa(ipaddr)); 
    } else {
        // Mensagem de erro em caso de falha
        printf("Falha na execução do DNS .\n");
    }
}

// Função para iniciar a resolução de DNS do broker MQTT de forma assíncrona
void run_dns_lookup(MQTT_CLIENT_T *state) {
    printf("Running DNS lookup for %s...\n", MQTT_SERVER_HOST);
    // Solicita a resolução de DNS para o host do broker MQTT
    if (dns_gethostbyname(MQTT_SERVER_HOST, &(state->remote_addr), dns_found, state) == ERR_INPROGRESS) {
        // Aguarda até que o IP seja resolvido (estado remoto ainda é 0)
        while (state->remote_addr.addr == 0) {
            cyw43_arch_poll(); // Mantém a pilha de rede atualizada
            sleep_ms(10); // Pequeno delay para não travar o sistema
        }
    }
}


// Callback chamado após tentativa de conexão ao broker
static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status){
    // Analisa o status da conexão
    if (status == MQTT_CONNECT_ACCEPTED) {
        printf("Comunicação MQTT criada com sucesso.\n");
    } else {
        printf("Falha ao criar a conexão MQTT: %d\n", status);
    }
}

// Callback chamado após uma solicitação de publicação 
void mqtt_pub_request_cb(void *arg, err_t err) {
    printf("Status da requisição de publicação: %d\n", err);
}

static void mqtt_pub_start_cb(void *arg, const char *topic, u32_t tot_len) {
    printf("Incoming message on topic: %s\n", topic);
}

void mqtt_sub_request_cb(void *arg, err_t err) {
    printf("Subscription request status: %d\n", err);
}

// Função que inicia a conexão com o broker MQTT
err_t mqtt_connect_client(MQTT_CLIENT_T *state) {
    // Estrutura com info do cliente
    struct mqtt_connect_client_info_t ci = {0};  
    ci.client_id = "mqttx_355aa4d4";  // ID do dispositivo no broker
    return mqtt_client_connect(state->mqtt_client, &(state->remote_addr),MQTT_SERVER_PORT, mqtt_connection_cb, state, &ci );
}




// função que envia uma mensagem para o broker (publica no tópico 'tipo_w/test') 
err_t publish(MQTT_CLIENT_T *state) {
    // Pega o tempo atual em milissegundos desde o boot
    uint32_t now = to_ms_since_boot(get_absolute_time());
    if (now - last_time >= PUB_DELAY_MS){
        // Armazena a data e hora da última publicação
        last_time = now;
        // Cria um buffer para a mensagem
        char buffer[BUFFER_SIZE];
        button_monitor(button_message);
        snprintf(buffer, BUFFER_SIZE, button_message);
        mqtt_publish(state->mqtt_client, "pico_w/pinstatus", buffer, strlen(buffer), 0, 0, mqtt_pub_request_cb, state);

        float temperatura = temperature_sensor_read();
        snprintf(buffer, BUFFER_SIZE, "Temperatura: %f",temperatura);
        return mqtt_publish(state->mqtt_client, "pico_w/sensortemp", buffer, strlen(buffer), 0, 0, mqtt_pub_request_cb, state);
    }
    else
    {
        return 0;
    }
}

static void mqtt_pub_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags) {
    char buffer[BUFFER_SIZE];
    if (len < BUFFER_SIZE) {
        memcpy(buffer, data, len);
        buffer[len] = '\0';
        printf("Message received: %s\n", buffer);
    } else {
        printf("Message too large, discarding.\n");
    }
}

void mqtt_run(MQTT_CLIENT_T *state) {
    // Cria nova instância de cliente MQTT
    state->mqtt_client = mqtt_client_new();
    // Verifica se o cliente foi criado com sucesso
    if (!state->mqtt_client) {
        printf("Falha na criação do cliente MQTT.\n");
        return;
    }

    if (mqtt_connect_client(state) == ERR_OK){
        mqtt_set_inpub_callback(state->mqtt_client, mqtt_pub_start_cb, mqtt_pub_data_cb, NULL);
        mqtt_sub_unsub(state->mqtt_client, "pico_w/recv", 0, mqtt_sub_request_cb, NULL, 1);
        // Entra no loop de eventos do cliente MQTT
        while (1) {
            cyw43_arch_poll();
            if (mqtt_client_is_connected(state->mqtt_client)) {
                printf("mensagem enviado.\n");
                publish(state);
                
                sleep_ms(50);
            } else {
                printf("Tentando reconectar...\n");
                sleep_ms(5000);
                mqtt_connect_client(state);
            }
        }
    }
}