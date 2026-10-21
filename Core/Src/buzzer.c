#include "buzzer.h"

extern TIM_HandleTypeDef htim2;

#define BUZZER_TIM       (&htim2)
#define BUZZER_CHANNEL   TIM_CHANNEL_3

// TIM 카운터 클록 1MHz 가정
#define BUZZER_TIMER_CLK 1000000U

#define BUZZER_INPUT_FREQ    2000U
#define BUZZER_SUCCESS_FREQ  2500U
#define BUZZER_FAILED_FREQ   1000U


typedef struct {
    uint32_t start_tick;
    uint32_t duration_ms;
    uint8_t  active;
} Buzzer_t;

static Buzzer_t buzzer;

// PWM 출력 시작
static void Buzzer_Start(uint32_t frequency, uint32_t duration_ms)
{
    if (frequency == 0U)
        return;

    uint32_t period = BUZZER_TIMER_CLK / frequency;

    if (period == 0U)
        return;

    __HAL_TIM_SET_AUTORELOAD(BUZZER_TIM, period - 1U);
    __HAL_TIM_SET_COUNTER(BUZZER_TIM, 0U);

    // Duty cycle 50%
    __HAL_TIM_SET_COMPARE(BUZZER_TIM, BUZZER_CHANNEL, period / 2U);

    HAL_TIM_PWM_Start(BUZZER_TIM, BUZZER_CHANNEL);

    buzzer.start_tick   = HAL_GetTick();
    buzzer.duration_ms  = duration_ms;
    buzzer.active       = 1;
}

// PWM 출력 중지
void Buzzer_Stop(void)
{
    HAL_TIM_PWM_Stop(BUZZER_TIM, BUZZER_CHANNEL);
    buzzer.active = 0;
}

// 부저 초기화
void Buzzer_Init(void)
{
	Buzzer_Stop();
}

// 입력 상태별 효과음
void Buzzer_On(eINPUT_STATE state)
{
    switch (state)
    {
        case eINPUT_STATE_INPUT:
            // 짧은 입력 효과음 1회
        	Buzzer_Start(BUZZER_INPUT_FREQ, 100);
            break;

        case eINPUT_STATE_SUCCESS:
            // 짧은 성공 효과음 2회
        	Buzzer_Start(BUZZER_SUCCESS_FREQ, 100);
            HAL_Delay(100);
            Buzzer_Stop();
            HAL_Delay(100);
            Buzzer_Start(BUZZER_SUCCESS_FREQ, 100);
            break;

        case eINPUT_STATE_FAILED:
            // 3초 연속 경고음
        	Buzzer_Start(BUZZER_FAILED_FREQ, 2000);
            break;

        default:
            break;
    }
}

void Buzzer_Update(void)
{
    if (buzzer.active && (uint32_t)(HAL_GetTick() - buzzer.start_tick) >= buzzer.duration_ms){
    	Buzzer_Stop();
    	return;
    }
}
