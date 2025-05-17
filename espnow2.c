// ESP-NOW RX - receive message for ESP IDF version 5.0 and higher

#include <string.h>
#include <stdio.h>
#include "esp_now.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"

static void on_data_recv(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len)
{
    uint8_t *mac = recv_info->src_addr;
    printf("Received from MAC %02X:%02X:%02X:%02X:%02X:%02X: %.*s\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], len, (char *)data);
}

void wifi_init()
{
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();
    esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
}

void app_main(void)
{
    wifi_init();

    esp_now_init();

    // Find local MAC address
    uint8_t mac_l[6] = {0};
    esp_read_mac(mac_l, ESP_MAC_WIFI_STA);
    printf("Local MAC address: \"0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\" \n", mac_l[0], mac_l[1], mac_l[2], mac_l[3], mac_l[4], mac_l[5]);

    // ESP-NOW
    esp_now_register_recv_cb(on_data_recv);
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
