#include "led.h"
#include "main.h"

void led_set_state(eLOCK_STATE state)
{
    if (state == eLOCK_STATE_LOCK)
    {
        // 빨간색 ON
    	HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_SET);
        // 파란색 OFF
    	HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, GPIO_PIN_RESET);
    }
    else if (state == eLOCK_STATE_UNLOCK)
    {
        // 빨간색 OFF
    	HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_RESET);
        // 파란색 ON
    	HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, GPIO_PIN_SET);
    }
}
