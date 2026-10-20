#ifndef INC_LCD1602_I2C_H_
#define INC_LCD1602_I2C_H_

#include <stdbool.h>
#include <stdint.h>
#include "main.h"

// LCD 초기화
HAL_StatusTypeDef LCD_I2C_Init(void);

// LCD 화면 초기화
HAL_StatusTypeDef LCD_I2C_Clear(void);

// LCD 커서 위치 설정
HAL_StatusTypeDef LCD_I2C_SetCursor(uint8_t col, uint8_t row);

// LCD 문자열 출력
HAL_StatusTypeDef LCD_I2C_Print(const char *str);

#endif /* INC_LCD1602_I2C_H_ */
