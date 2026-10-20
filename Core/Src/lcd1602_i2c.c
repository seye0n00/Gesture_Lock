#include "lcd1602_i2c.h"

extern I2C_HandleTypeDef hi2c1;

#define LCD1602_ADDR  (0x27 << 1)

// PCF8574 LCD 백팩 제어 비트
#define LCD_RS  0x01
#define LCD_RW  0x02
#define LCD_EN  0x04
#define LCD_BL  0x08

static uint8_t backlight = LCD_BL;


// 내부 함수 선언
static HAL_StatusTypeDef LCD_I2C_WriteByte(uint8_t data);
static HAL_StatusTypeDef LCD_I2C_EnablePulse(uint8_t data);
static HAL_StatusTypeDef LCD_I2C_Write4Bits(uint8_t data);
static HAL_StatusTypeDef LCD_I2C_SendCommand(uint8_t cmd);
static HAL_StatusTypeDef LCD_I2C_SendData(uint8_t data);


// PCF8574에 1바이트 전송
static HAL_StatusTypeDef LCD_I2C_WriteByte(uint8_t data)
{
    return HAL_I2C_Master_Transmit(&hi2c1, LCD1602_ADDR, &data, 1, 100);
}


// LCD Enable 펄스 생성
static HAL_StatusTypeDef LCD_I2C_EnablePulse(uint8_t data)
{
    HAL_StatusTypeDef status;

    status = LCD_I2C_WriteByte(data | LCD_EN);
    if (status != HAL_OK)
        return status;

    return LCD_I2C_WriteByte(data & ~LCD_EN);
}


// 상위 또는 하위 4비트 전송
static HAL_StatusTypeDef LCD_I2C_Write4Bits(uint8_t data)
{
    HAL_StatusTypeDef status;

    status = LCD_I2C_WriteByte(data);
    if (status != HAL_OK)
        return status;

    return LCD_I2C_EnablePulse(data);
}


// LCD 제어 명령 전송
static HAL_StatusTypeDef LCD_I2C_SendCommand(uint8_t cmd)
{
    HAL_StatusTypeDef status;

    uint8_t high = cmd & 0xF0;
    uint8_t low  = (cmd << 4) & 0xF0;

    status = LCD_I2C_Write4Bits(high | backlight);
    if (status != HAL_OK)
        return status;

    status = LCD_I2C_Write4Bits(low | backlight);
    if (status != HAL_OK)
        return status;

    HAL_Delay(1);

    return HAL_OK;
}


// LCD 문자 데이터 전송
static HAL_StatusTypeDef LCD_I2C_SendData(uint8_t data)
{
    HAL_StatusTypeDef status;

    uint8_t high = data & 0xF0;
    uint8_t low  = (data << 4) & 0xF0;

    status = LCD_I2C_Write4Bits(high | LCD_RS | backlight);

    if (status != HAL_OK)
        return status;

    status = LCD_I2C_Write4Bits(low | LCD_RS | backlight);

    if (status != HAL_OK)
        return status;

    HAL_Delay(1);

    return HAL_OK;
}


// LCD 초기화
HAL_StatusTypeDef LCD_I2C_Init(void)
{
    HAL_StatusTypeDef status;

    HAL_Delay(50);

    // HD44780 4비트 초기화 시퀀스
    status = LCD_I2C_Write4Bits(0x30 | backlight);
    if (status != HAL_OK) return status;
    HAL_Delay(5);

    status = LCD_I2C_Write4Bits(0x30 | backlight);
    if (status != HAL_OK) return status;
    HAL_Delay(1);

    status = LCD_I2C_Write4Bits(0x30 | backlight);
    if (status != HAL_OK) return status;
    HAL_Delay(1);

    status = LCD_I2C_Write4Bits(0x20 | backlight);
    if (status != HAL_OK) return status;
    HAL_Delay(1);

    // 4비트, 2라인, 5x8 폰트
    status = LCD_I2C_SendCommand(0x28);
    if (status != HAL_OK) return status;

    // 디스플레이 OFF
    status = LCD_I2C_SendCommand(0x08);
    if (status != HAL_OK) return status;

    // 화면 초기화
    status = LCD_I2C_Clear();
    if (status != HAL_OK) return status;

    // 커서 오른쪽 이동 설정
    status = LCD_I2C_SendCommand(0x06);
    if (status != HAL_OK) return status;

    // 디스플레이 ON, 커서 OFF
    return LCD_I2C_SendCommand(0x0C);
}


// LCD 화면 초기화
HAL_StatusTypeDef LCD_I2C_Clear(void)
{
    HAL_StatusTypeDef status;

    status = LCD_I2C_SendCommand(0x01);

    if (status == HAL_OK)
        HAL_Delay(2);

    return status;
}


// LCD 커서 위치 설정
HAL_StatusTypeDef LCD_I2C_SetCursor(uint8_t col, uint8_t row)
{
    if (col >= 16 || row >= 2)
        return HAL_ERROR;

    uint8_t addr = (row == 0) ? 0x00 : 0x40;

    return LCD_I2C_SendCommand(0x80 | (addr + col));
}


// LCD 문자열 출력
HAL_StatusTypeDef LCD_I2C_Print(const char *str)
{
    if (str == NULL)
        return HAL_ERROR;

    while (*str)
    {
        HAL_StatusTypeDef status;

        status = LCD_I2C_SendData((uint8_t)*str);

        if (status != HAL_OK)
            return status;

        str++;
    }

    return HAL_OK;
}
