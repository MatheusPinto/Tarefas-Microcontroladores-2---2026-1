#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_private/esp_clk.h"
#include "driver/mcpwm_cap.h"
#include "driver/gpio.h"
#include "threeeyes.h"
#include <inttypes.h>

extern TaskHandle_t ir_line_task_handle; //para a terefa treeeye_task.c poder suspender a ir_line_task

portTASK_FUNCTION(IR_Line, arg);