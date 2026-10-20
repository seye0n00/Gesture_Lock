#ifndef INC_MPU6050_I2C_H_
#define INC_MPU6050_I2C_H_

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"
#include <stdint.h>

HAL_StatusTypeDef MPU6050_Init(void);
HAL_StatusTypeDef MPU6050_ReadAccel(int16_t *ax, int16_t *ay, int16_t *az);

#endif /* INC_MPU6050_I2C_H_ */
