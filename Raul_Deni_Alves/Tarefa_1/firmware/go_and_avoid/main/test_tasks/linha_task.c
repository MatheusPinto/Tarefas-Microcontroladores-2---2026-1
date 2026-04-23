#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_rom_gpio.h"

static const char *TAG = "LINHA";

#define SENSOR_A_ESQ      GPIO_NUM_15
#define SENSOR_B_ESQ_INT  GPIO_NUM_16
#define SENSOR_C_MEIO     GPIO_NUM_17
#define SENSOR_D_DIR_INT  GPIO_NUM_18
#define SENSOR_E_DIR      GPIO_NUM_8

void vTaskLinha(void *pvParameters) {
    QueueHandle_t fila = (QueueHandle_t)pvParameters;
    
    // Setup dos pinos dos sensores
    esp_rom_gpio_pad_select_gpio(SENSOR_A_ESQ);
    esp_rom_gpio_pad_select_gpio(SENSOR_B_ESQ_INT);
    esp_rom_gpio_pad_select_gpio(SENSOR_C_MEIO);
    esp_rom_gpio_pad_select_gpio(SENSOR_D_DIR_INT);
    esp_rom_gpio_pad_select_gpio(SENSOR_E_DIR);

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL<<SENSOR_A_ESQ) | (1ULL<<SENSOR_B_ESQ_INT) | (1ULL<<SENSOR_C_MEIO) | (1ULL<<SENSOR_D_DIR_INT) | (1ULL<<SENSOR_E_DIR),
        .mode = GPIO_MODE_INPUT,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    // Inicia o movimento pra frente
    int comando_atual = 1; 

    while(1) {
        int a = gpio_get_level(SENSOR_A_ESQ);
        int b = gpio_get_level(SENSOR_B_ESQ_INT);
        int c = gpio_get_level(SENSOR_C_MEIO);
        int d = gpio_get_level(SENSOR_D_DIR_INT);
        int e = gpio_get_level(SENSOR_E_DIR);
        
        // Centro na linha -> Segue reto
        if (c == 0) {
            comando_atual = 1; 
        } 
        // Correções leves
        else if (b == 0) {
            comando_atual = 3; // Esquerda leve
        } else if (d == 0) {
            comando_atual = 2; // Direita leve
        } 
        // Correções bruscas (pontas)
        else if (a == 0) {
            comando_atual = 5; // Esquerda brusca
        } else if (e == 0) {
            comando_atual = 4; // Direita brusca
        }

        // Envia pra fila e aguarda 20ms
        xQueueSend(fila, &comando_atual, 0);
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}