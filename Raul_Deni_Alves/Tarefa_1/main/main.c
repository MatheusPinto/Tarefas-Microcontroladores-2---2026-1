/*
 * Arquivo Principal - Robô Juca
 */

#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_private/esp_clk.h"
#include "driver/mcpwm_cap.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "driver/pulse_cnt.h"
#include "bdc_motor.h"
#include "pid_ctrl.h"
#include "hal/gpio_types.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "wheel.h"
#include "treeeyes_task.h"
#include "wheel_task.h"
#include "imu_task.h"
#include "linha_task.h"
#include <inttypes.h>

// Handles globais
TaskHandle_t xHandleLinha = NULL;
QueueHandle_t xFilaComandos;

// Tarefas ativas no build
#define TREE_EYES_TASK
#define WHEEL_CTRL_TASK
#define LINHA_TASK

void app_main(void) {
    // Cria fila de comunicação
    xFilaComandos = xQueueCreate(5, sizeof(int));

    // Tarefa dos Motores 
    xTaskCreate(wheel_ctrl, "wheel", 4096, (void *)xFilaComandos, 4, NULL);

    // Tarefa da Linha - Guarda o handle para a segurança usar depois
    xTaskCreate(vTaskLinha, "linha", 4096, (void *)xFilaComandos, 5, &xHandleLinha);

    // Tarefa de Segurança (Prioridade máxima)
    xTaskCreate(Treeeyes, "safety", 4096, (void *)xFilaComandos, 10, NULL);
}