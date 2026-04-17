/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
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
#include "threeeyes_task.h"
#include "wheel_task.h"
#include "imu_task.h"
#include <inttypes.h>

//----------------------
#include "ir_line_task.h"
//----------------------

#define THREE_EYES_TASK
#define IMU_TASK
//#define WHEEL_CTRL_TASK
#define IR_LINE_TASK

void app_main(void)
{
#ifdef THREE_EYES_TASK
    xTaskCreate(Threeeyes,
                "threeeyes",
                configMINIMAL_STACK_SIZE*3,
                NULL,
                5,
                NULL);
#endif

//======================================

#ifdef IR_LINE_TASK
    xTaskCreate(IR_Line,
                "IR_Line",
                configMINIMAL_STACK_SIZE*4,
                NULL,
                5,
                //NULL); //comentar esta linha quando for testar o taskSuspend e descomentar a linha de baixo
                &ir_line_task_handle);  //para a terefa treeeye_task.c poder suspender a ir_line_task quando detectar objeto
#endif
//=========================================

#ifdef IMU_TASK
    xTaskCreate(IMU_Task,
                "imu",
                configMINIMAL_STACK_SIZE*3,
                NULL,
                5,
                NULL);
#endif

#ifdef WHEEL_CTRL_TASK
    xTaskCreate(wheel_ctrl,
                "wheel",
                configMINIMAL_STACK_SIZE*3,
                NULL,
                5,
                NULL);
#endif
}