#include "lockdisp.h"
#include "lcd1602_i2c.h"
#include "system_types.h"

#include <stdint.h>
#include <string.h>

#define LCD_COLS            (16U)

#define LOCK_DISPLAY_ROW    (0U)
#define INPUT_DISPLAY_ROW   (1U)

#define DEFAULT_INPUT_POS   (4U)
#define MAX_INPUT_POS       (15U)


// 시스템 상태 변수
extern eLOCK_STATE lock_state;
extern eINPUT_STATE input_state;


// 잠금 상태별 출력 문자열
static const char *const lock_state_print[eLOCK_STATE_MAX] = {
    [eLOCK_STATE_LOCK]   = "[ LOCKED ]",
    [eLOCK_STATE_UNLOCK] = "[ UNLOCKED ]"
};


// 입력 상태별 출력 문자열
static const char *const input_state_print[eINPUT_STATE_MAX] = {
    [eINPUT_STATE_INPUT]   = "[PW|           ]",
    [eINPUT_STATE_SUCCESS] = "[ UNLOCKED ]",
    [eINPUT_STATE_FAILED]  = "[  FAILED!  ]"
};


// 다음 입력 문자를 표시할 열 위치
static size_t input_pos = DEFAULT_INPUT_POS;


// 특정 행 전체를 공백으로 지움
static void lockdisp_clear_row(uint8_t row)
{
    if (LCD_I2C_SetCursor(0, row) != HAL_OK)
        return;

    LCD_I2C_Print("                ");
}


// 잠금 화면 초기화
bool lockdisp_init(void)
{
    if (LCD_I2C_Init() != HAL_OK)
        return false;

    if (LCD_I2C_Clear() != HAL_OK)
        return false;

    // 첫 번째 행: 잠금 상태
    if (lock_state < eLOCK_STATE_MAX)
    {
        LCD_I2C_SetCursor(0, LOCK_DISPLAY_ROW);
        LCD_I2C_Print(lock_state_print[lock_state]);
    }

    // 두 번째 행: 입력 상태
    if (input_state < eINPUT_STATE_MAX)
    {
        LCD_I2C_SetCursor(0, INPUT_DISPLAY_ROW);
        LCD_I2C_Print(input_state_print[input_state]);
    }

    input_pos = DEFAULT_INPUT_POS;
    return true;
}


// 잠금 상태 화면 갱신
void lockdisp_update_lock(void)
{
    if (lock_state >= eLOCK_STATE_MAX)
        return;

    // 잠금 상태 변경 시 깜빡임 효과
    for (int i = 0; i < 3; i++)
    {
        lockdisp_clear_row(LOCK_DISPLAY_ROW);
        HAL_Delay(50);

        LCD_I2C_SetCursor(0, LOCK_DISPLAY_ROW);
        LCD_I2C_Print(lock_state_print[lock_state]);
        HAL_Delay(50);
    }
}


// 비밀번호 입력 상태 화면 갱신
void lockdisp_update_input(void)
{
    if (input_state >= eINPUT_STATE_MAX)
        return;

    // 기존 입력 내용 제거
    lockdisp_clear_row(INPUT_DISPLAY_ROW);

    // 변경된 입력 상태 표시
    LCD_I2C_SetCursor(0, INPUT_DISPLAY_ROW);
    LCD_I2C_Print(input_state_print[input_state]);

    // 다음 입력을 위한 위치 초기화
    input_pos = DEFAULT_INPUT_POS;
}


// 비밀번호 제스처 한 자리 표시
void lockdisp_next_input(const char *str, size_t len)
{
    if (str == NULL || len == 0)
        return;

    // 비밀번호 입력 상태에서만 허용
    if (input_state != eINPUT_STATE_INPUT)
        return;

    // 마지막 ']' 문자를 위한 공간 확보
    if (input_pos >= MAX_INPUT_POS || len > MAX_INPUT_POS - input_pos)
        return;

    char buffer[LCD_COLS + 1];

    memcpy(buffer, str, len);
    buffer[len] = '\0';

    if (LCD_I2C_SetCursor((uint8_t)input_pos, INPUT_DISPLAY_ROW) != HAL_OK)
        return;

    if (LCD_I2C_Print(buffer) != HAL_OK)
        return;

    input_pos += len;
}
