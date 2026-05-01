#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "wheel.h"
#include "freertos/queue.h"

const static char *TAG = "wheels";

portTASK_FUNCTION(wheel_ctrl, arg)
{
    // Inicializa os drivers de motor e garante que o robô comece parado
    wheel_Init();
    wheel_SetVel(0, 0);

    int comando_recebido;
    
    // Definição das velocidades de operação
    const int vel_cruzeiro = 350;    // Velocidade base para reta
    const int vel_curva_leve = 300;  // Velocidade da roda  em curvas suaves
    const int vel_curva_forte = 300; // Velocidade da roda  em curvas fechadas

    ESP_LOGI(TAG, "Tarefa de Motores pronta.");

    while(1)
    {
        // Aguarda a chegada de um novo comando pela fila (fica bloqueada até receber)
        if (xQueueReceive((QueueHandle_t)arg, &comando_recebido, portMAX_DELAY))
        {
            switch(comando_recebido) {
                case 1: // Comando para seguir em frente
                    wheel_GoForward();
                    wheel_SetVel(vel_cruzeiro, vel_cruzeiro);
                    break;
                
                case 2: // Curva suave para esquerda (reduz roda esquerda)
                    wheel_GoForward();
                    wheel_SetVel(0, vel_curva_leve); 
                    break;

                case 3: // Curva suave para direita (reduz roda direita)
                    wheel_GoForward();
                    wheel_SetVel(vel_curva_leve, 0);
                    break;

                case 4: // Curva brusca para esquerda
                    wheel_GoForward();
                    wheel_SetVel(0, vel_curva_forte);
                    break;

                case 5: // Curva brusca para direita
                    wheel_GoForward();
                    wheel_SetVel(vel_curva_forte, 0);
                    break;

                case 0: // Comando de parada (usado pela segurança ou perda de linha)
                default:
                    wheel_SetVel(0, 0);
                    break;
            }
            
            ESP_LOGD(TAG, "Executando CMD: %d", comando_recebido);
        }
    }
}