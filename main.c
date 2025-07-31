#include <stdio.h>
#include "pico/stdlib.h"

#include "lib/wifiConnection.h"
#include "lib/mqttConnection.h"
#include "lib/temperaturesensor.h"

#define SSID "Infitech"
#define PASSWORD "infitech#dpm"

// Inicializa a estrututa de dados do cliente MQTT alocando espaço na memória
static MQTT_CLIENT_T* mqtt_client_init(void) {
    // Aloca e zera a estrutura do cliente
    MQTT_CLIENT_T *state = calloc(1, sizeof(MQTT_CLIENT_T));
    // Analisa se a alocação foi bem-sucedida

    if (!state) {
        // Mensagem de erro caso a alocação falhe
        printf("Falha ao alocar memória da estrutura da dados do cliente MQTT.\n");
        return NULL;
    }
    // Retorna ponteiro para estrutura inicializada
    return state;
}

int main()
{
    stdio_init_all();
    init_sensor();
    sleep_ms(10000);
    if (!wifi_init()) {
        return false;
    }
    // inicializa a conexão Wi-Fi
    while (!wifi_connect(SSID, PASSWORD)) {
        sleep_ms(8000);  // Espera 1 segundo antes de tentar novamente
        
    }

    MQTT_CLIENT_T *mqtt_state = mqtt_client_init();

    run_dns_lookup(mqtt_state);
    
    mqtt_run(mqtt_state);

    cyw43_arch_deinit();  // Desliga o Wi-Fi (não será chamado, pois o loop é infinito)
}
