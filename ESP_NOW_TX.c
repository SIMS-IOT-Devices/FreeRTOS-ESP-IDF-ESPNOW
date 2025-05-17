// ESP NOW TX - send message for ESP IDF version 5.0 and higher

#include <string.h>
#include <stdio.h>
#include "esp_now.h"
#include "esp_mac.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Receiver MAC Address
uint8_t mac_destination[6] = {0xec, 0x62, 0x60, 0x9b, 0x8b, 0x38};

// Callback function
void on_data_sent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    printf("Sent to MAC %02X:%02X:%02X:%02X:%02X:%02X - Status: %s\n",
           mac_addr[0], mac_addr[1], mac_addr[2],
           mac_addr[3], mac_addr[4], mac_addr[5],
           status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
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
    esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE); // WiFi on channel 1
}

void app_main(void)
{
    wifi_init();

    // Find local mac address
    uint8_t my_esp_mac[6] = {0x94, 0xB5, 0x55, 0xC9, 0x71, 0xD0};
    esp_read_mac(my_esp_mac, ESP_MAC_WIFI_STA);
    printf("Local MAC address: %02X:%02X:%02X:%02X:%02X:%02X\n",
           my_esp_mac[0], my_esp_mac[1], my_esp_mac[2],
           my_esp_mac[3], my_esp_mac[4], my_esp_mac[5]);

    // ESP-NOW initiation and register a callback function that will be called
    esp_now_init();
    esp_now_register_send_cb(on_data_sent);

    // Add a peer device with which ESP32 can communicate
    esp_now_peer_info_t peer = {0};
    memcpy(peer.peer_addr, mac_destination, 6);
    peer.channel = 1;
    peer.encrypt = false;
    esp_now_add_peer(&peer);

    // Send the message
    const char *message = "Hello via ESP-NOW";
    while (1)
    {
        esp_now_send(mac_destination, (uint8_t *)message, strlen(message));
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
