#include "treeeyes_task.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"

// Handle da tarefa de linha definido no main.c
extern TaskHandle_t xHandleLinha; 

portTASK_FUNCTION(Treeeyes, args)
{
    QueueHandle_t fila = (QueueHandle_t)args;
    TreeEyes_Init();
    ultrasonic_value_t sensor[3];
    int comando_parar = 0;

    while(1)
    {
        // Dispara o pulso e lê os 3 sensores (Esq, Meio, Dir)
        TreeEyes_TrigAndWait(portMAX_DELAY);
        TreeEyes_Read(&sensor[0], &sensor[1], &sensor[2]);
        
        float menor_distancia = 1000.0; // Inicia com valor alto para comparação

        // Loop para encontrar o objeto mais próximo entre os 3 sensores
        for (int i = 0; i < 3; i++) {
            // Converte ticks para centímetros
            float dist_sensor = (sensor[i].tof_ticks * (1000000.0 / esp_clk_apb_freq())) / 58.0;

            // Se a leitura for válida e menor que a anterior, atualiza a menor distância
            if (dist_sensor > 0 && dist_sensor < menor_distancia) {
                menor_distancia = dist_sensor;
            }
        }

        // Lógica de frenagem (10 cm de limite)
        if (menor_distancia <= 10.0) 
        {
            // Pausa a navegação e limpa comandos pendentes
            if (xHandleLinha != NULL) vTaskSuspend(xHandleLinha);
            
            xQueueReset(fila); 
            xQueueSend(fila, &comando_parar, 0); 
            ESP_LOGW("SEGURANÇA", "Objeto detectado a %.2f cm! Parando...", menor_distancia);
        }
        else 
        {
            // Caminho livre: retoma a tarefa de linha
            if (xHandleLinha != NULL) vTaskResume(xHandleLinha);
        }
    
        // Intervalo entre leituras
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}