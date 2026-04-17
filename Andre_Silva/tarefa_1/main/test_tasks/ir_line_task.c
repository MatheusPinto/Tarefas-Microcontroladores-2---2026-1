#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
//#include "led_strip.h"
#include "sdkconfig.h"
#include "soc/gpio_reg.h"

#include "ir_line_task.h"
#include "wheel.h" //Pra eu usar o wheel_SetVel(Vel_rodaEsquerda, Vel_rodaDireita)
#include "wheel.c"
static const char *TAG = "IR_Line";

#define INFRA_RED_VERY_LEFT_GPIO  15 //15
#define INFRA_RED_LEFT_GPIO  16  //16
#define INFRA_RED_MIDDLE_GPIO  17 //17
#define INFRA_RED_RIGHT_GPIO  18 //18
#define INFRA_RED_VERY_RIGHT_GPIO 8

#define IR_LINE_VERY_VERY_LEFT (1ULL << INFRA_RED_VERY_LEFT_GPIO)
#define IR_LINE_VERY_LEFT ((1ULL << INFRA_RED_VERY_LEFT_GPIO) | (1ULL << INFRA_RED_LEFT_GPIO))
#define IR_LINE_LEFT (1ULL << INFRA_RED_LEFT_GPIO)
#define IR_LINE_LEFT_MIDDLE ((1ULL << INFRA_RED_LEFT_GPIO) | (1ULL << INFRA_RED_MIDDLE_GPIO))
#define IR_LINE_MIDDLE (1ULL << INFRA_RED_MIDDLE_GPIO)
#define IR_LINE_RIGHT_MIDDLE ((1ULL << INFRA_RED_MIDDLE_GPIO) | (1ULL << INFRA_RED_RIGHT_GPIO))
#define IR_LINE_RIGHT (1ULL << INFRA_RED_RIGHT_GPIO)
#define IR_LINE_VERY_RIGHT ((1ULL << INFRA_RED_RIGHT_GPIO) | (1ULL << INFRA_RED_VERY_RIGHT_GPIO))
#define IR_LINE_VERY_VERY_RIGHT (1ULL << INFRA_RED_VERY_RIGHT_GPIO)

#define INFRA_RED_OUT_GPIO_MASK ((uint64_t)((1ULL << INFRA_RED_VERY_LEFT_GPIO) | (1ULL << INFRA_RED_LEFT_GPIO) | (1ULL << INFRA_RED_MIDDLE_GPIO) | (1ULL << INFRA_RED_RIGHT_GPIO) | (1ULL << INFRA_RED_VERY_RIGHT_GPIO)))


// ========= Velocidades  =========
#define VEL_MAX      225  // Velocidade máxima  na faixa de 200~225 ficou bom. Velocidades mais rápidas fazem o carrinho sair da pista.
#define VEL_MIN      190  // Velocidade mínima na faixa de 170~190 ficou bom. Valores muito baixos não dá torque no carrinho e ele para com o próprio peso.


// ===================================

TaskHandle_t ir_line_task_handle = NULL; //handle para  usar o vTaskSuspend ou  vTaskResume no threeeyes_task.c

portTASK_FUNCTION(IR_Line, arg)
//void app_main()
{
	gpio_config_t ir_line_config = {
			.pin_bit_mask = INFRA_RED_OUT_GPIO_MASK,
			.mode = GPIO_MODE_INPUT,
			.pull_down_en = GPIO_PULLDOWN_DISABLE,
			.pull_up_en = GPIO_PULLUP_DISABLE,
			.intr_type = GPIO_INTR_DISABLE
	};

	gpio_config( &ir_line_config );

	//iniciar o robo
	 wheel_Init();
	 wheel_SetVel(BDC_MCPWM_DUTY_TICK_MAX, BDC_MCPWM_DUTY_TICK_MAX);
     wheel_GoForward();
	//---------------------------
	uint64_t gpioValue;


	while (1) {
		gpioValue = (uint64_t)gpio_get_level(INFRA_RED_VERY_LEFT_GPIO) << INFRA_RED_VERY_LEFT_GPIO |
				(uint64_t)gpio_get_level(INFRA_RED_LEFT_GPIO) << INFRA_RED_LEFT_GPIO|
				(uint64_t)gpio_get_level(INFRA_RED_MIDDLE_GPIO) << INFRA_RED_MIDDLE_GPIO|
				(uint64_t)gpio_get_level(INFRA_RED_RIGHT_GPIO) << INFRA_RED_RIGHT_GPIO|
				(uint64_t)gpio_get_level(INFRA_RED_VERY_RIGHT_GPIO) << INFRA_RED_VERY_RIGHT_GPIO;

		gpioValue &= INFRA_RED_OUT_GPIO_MASK;

        ESP_LOGI(TAG, "gpio value: %llu", gpioValue);
		//ESP_LOGI(TAG, "gpio_value: %x", gpioValue);
		//gpioValue = (REG_READ(GPIO_IN_REG) & INFRA_RED_OUT_GPIO_MASK);
		switch(gpioValue)
		{
		
		//O robô possui 5 sensores infravermelhos.
		//O valor de leitura de gpioValue é de acordo com os bits dos leitores --> 11011 = 360704
		//Os valores foram obtidos pelo terminal serial. Alguns estados não foram levados em consideração pois eles dificilmente acontecerão, como o estado 10000.Eles reagirão do acordo com o case default:
		// 0 -> sensor em cima da linha preta (não reflexivo)
		// 1 -> Sensor em cima da parte branca(refletindo)
		//wheel_SetVel(wL:roda_direita, wR:Roda Esquerda) --> A notação que aparece (wL: e wR:) está invertido
		case 360704://11011
			wheel_GoForward();
			ESP_LOGI(TAG, "Indo Reto 11011");
			wheel_SetVel(VEL_MAX, VEL_MAX);

			break;
			
		case 426240://10111
			wheel_GoRight();
			ESP_LOGI(TAG, "Soft Left 10111 ");
			wheel_SetVel(VEL_MAX, VEL_MIN);

			break;

		case 229632://11101
			wheel_GoLeft();
			ESP_LOGI(TAG, "Soft RIgth 11101");
			wheel_SetVel(VEL_MIN, VEL_MAX);

			break;
	
		case 459008://01111
			wheel_GoForward();
			ESP_LOGI(TAG, "HArd Left 01111");
			wheel_SetVel(VEL_MAX, 0);

			break;

						
		case 491520://11110
			wheel_GoForward();
			ESP_LOGI(TAG, "HardRight 11110");
			wheel_SetVel(0, VEL_MAX);

			break;
					
		case 229376://11100
			wheel_GoLeft();
			ESP_LOGI(TAG, "HardRight 11100");
			wheel_SetVel(VEL_MIN, VEL_MIN);

			break;
			
									
		case 393472://00111
			wheel_GoRight();
			ESP_LOGI(TAG, "HardLeft 00111");
			wheel_SetVel(VEL_MIN, VEL_MIN); 
			
			break;
			
									
		case 98304://11000
			wheel_GoLeft();
			ESP_LOGI(TAG, "HardRight 11000");
			wheel_SetVel(VEL_MIN, VEL_MIN); 
			
			break;
			
									
		case 262400://00011
			wheel_GoRight();
			ESP_LOGI(TAG, "HardLeft 00011");
			wheel_SetVel(VEL_MIN, VEL_MIN); //(VEL_MIN, 0)
			
			break;
			
		case 295168://10011
			wheel_GoForward();
			ESP_LOGI(TAG, "HardLeft 10011");
			wheel_SetVel(VEL_MIN, 0);
			break;
			
		case 98560://11001
			wheel_GoForward();
			ESP_LOGI(TAG, "HardLeft 11001");
			wheel_SetVel(VEL_MIN, 0);
			
			break;
			
			
		case 491776://11111 Cantinua andando até encontrar a pista
			wheel_GoForward();
			ESP_LOGI(TAG, "Looking 11111");
			wheel_SetVel(VEL_MIN, VEL_MIN);
			
			
			
			break;
			


		default:
			ESP_LOGI(TAG, "NOTHING!");
			wheel_SetVel(0, 0);
			
			break;
		}
        //vTaskDelay( 500 / portTICK_PERIOD_MS);
		vTaskDelay( pdMS_TO_TICKS(50));
    }
}
