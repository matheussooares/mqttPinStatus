#ifndef MQTTCONNECTION_H
#define MQTTCONNECTION_H
    
    #include "lwip/dns.h"
    #include "lwip/tcp.h"
    #include "lwip/pbuf.h"

    #include "lwip/apps/mqtt.h"
    #include "lwip/altcp_tcp.h"
    #include "lwip/altcp_tls.h"
    #include "pico/cyw43_arch.h"
    #include "lwip/apps/mqtt_priv.h"
    #include "hardware/structs/rosc.h"

    #include <string.h>
    #include <time.h>
    #include <stdio.h>
    #include "button.h"
    #include "temperaturesensor.h"
    

    #define MQTT_SERVER_HOST "broker.emqx.io" // endereço do broker MQTT  "test.mosquitto.org"
    #define MQTT_SERVER_PORT 1883 // porta do broker MQTT

    #define BUFFER_SIZE 256 // Tamanho do buffer para publicar mensagens MQTT
    #define PUB_DELAY_MS 1000 // Tempo de publicação em milissegundos

    
    static uint32_t last_time = 0;

    // Definição da estrutura de estado do cliente MQTT
    typedef struct MQTT_CLIENT_T_ {
        ip_addr_t remote_addr; // IP do broker MQTT
        mqtt_client_t *mqtt_client; // Ponteiro para o cliente MQTT (lwIP)
        u32_t received;// Contador de mensagens recebidas
        u32_t counter; // Contador de mensagens publicadas
        u32_t reconnect;// Contador de reconexões
    } MQTT_CLIENT_T;

    // // Inicializa a estrututa de dados do cliente MQTT alocando espaço na memória
    // static MQTT_CLIENT_T* mqtt_client_init(void); 
    // Função de callback chamada quando a resolução de DNS terminar
    void dns_found(const char *name, const ip_addr_t *ipaddr, void *callback_arg);
    // Função para iniciar a resolução de DNS do broker MQTT de forma assíncrona
    void run_dns_lookup(MQTT_CLIENT_T *state);
    // Callback chamado após tentativa de conexão ao broker
    static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status);
    // Callback chamado após uma solicitação de publicação 
    void mqtt_pub_request_cb(void *arg, err_t err);
    // Função que inicia a conexão com o broker MQTT
    err_t mqtt_connect_client(MQTT_CLIENT_T *state) ;
    // função que envia uma mensagem para o broker (publica no tópico 'tipo_w/test') 
    err_t publish(MQTT_CLIENT_T *state);

    void mqtt_run(MQTT_CLIENT_T *state);


    void mqtt_sub_request_cb(void *arg, err_t err);

    static void mqtt_pub_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags);

    static void mqtt_pub_start_cb(void *arg, const char *topic, u32_t tot_len);


   
#endif
