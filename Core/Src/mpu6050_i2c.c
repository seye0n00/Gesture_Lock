#include "mpu6050_i2c.h"

extern I2C_HandleTypeDef hi2c1;

// MPU6050 I2C Address (AD0 = LOW)
#define MPU6050_ADDR              (0x68 << 1)


// MPU6050 Register Address
#define MPU6050_WHO_AM_I          0x75
#define MPU6050_PWR_MGMT_1        0x6B
#define MPU6050_PWR_MGMT_2        0x6C
#define MPU6050_ACCEL_CONFIG      0x1C
#define MPU6050_CONFIG            0x1A
#define MPU6050_SMPLRT_DIV        0x19


// Configuration Values
#define MPU6050_DEVICE_ID         0x68
#define MPU6050_PWR_MGMT_1_CFG    0x01
#define MPU6050_PWR_MGMT_2_CFG    0x00
#define MPU6050_ACCEL_CFG         0x00
#define MPU6050_DLPF_CFG          0x04
#define MPU6050_SAMPLE_RATE_CFG   0x09


#define MPU6050_TIMEOUT_MS        100


#define MPU6050_ACCEL_XOUT_H  0x3B

HAL_StatusTypeDef MPU6050_Init(void)
{
    HAL_StatusTypeDef status;
    uint8_t data;


    volatile uint32_t debug_state;
    volatile uint32_t debug_sr2;
    volatile uint32_t debug_scl;
    volatile uint32_t debug_sda;

    debug_state = hi2c1.State;
    debug_sr2 = I2C1->SR2;

    debug_scl = (GPIOB->IDR & GPIO_PIN_8) ? 1 : 0;
    debug_sda = (GPIOB->IDR & GPIO_PIN_9) ? 1 : 0;


    // Check Device ID
    status = HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, MPU6050_WHO_AM_I, I2C_MEMADD_SIZE_8BIT, &data, 1, MPU6050_TIMEOUT_MS);

    if (status != HAL_OK)
        return status;

    if (data != MPU6050_DEVICE_ID)
        return HAL_ERROR;


    // Wake up MPU6050
    data = MPU6050_PWR_MGMT_1_CFG;
    status = HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, MPU6050_PWR_MGMT_1, I2C_MEMADD_SIZE_8BIT, &data, 1, MPU6050_TIMEOUT_MS);

    if (status != HAL_OK)
        return status;

    HAL_Delay(10);


    // Enable all accelerometer and gyroscope axes
    data = MPU6050_PWR_MGMT_2_CFG;
    status = HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, MPU6050_PWR_MGMT_2, I2C_MEMADD_SIZE_8BIT, &data, 1, MPU6050_TIMEOUT_MS);


    if (status != HAL_OK)
        return status;

    // Set Accelerometer Range: ±2g
    data = MPU6050_ACCEL_CFG;
    status = HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, MPU6050_ACCEL_CONFIG, I2C_MEMADD_SIZE_8BIT, &data, 1, MPU6050_TIMEOUT_MS);


    if (status != HAL_OK)
        return status;

    // Configure DLPF: 21Hz
    data = MPU6050_DLPF_CFG;
    status = HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, MPU6050_CONFIG, I2C_MEMADD_SIZE_8BIT, &data, 1, MPU6050_TIMEOUT_MS);


    if (status != HAL_OK)
        return status;


    // Set Sample Rate: 100Hz
    data = MPU6050_SAMPLE_RATE_CFG;
    status = HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, MPU6050_SMPLRT_DIV, I2C_MEMADD_SIZE_8BIT, &data, 1, MPU6050_TIMEOUT_MS);

    if (status != HAL_OK)
        return status;

    return HAL_OK;
}

HAL_StatusTypeDef MPU6050_ReadAccel(int16_t *ax, int16_t *ay, int16_t *az)
{
    uint8_t data[6];


    if (ax == NULL || ay == NULL || az == NULL)
    {
        return HAL_ERROR;
    }


    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(
        &hi2c1,
        MPU6050_ADDR,
        MPU6050_ACCEL_XOUT_H,
        I2C_MEMADD_SIZE_8BIT,
        data,
        6,
        MPU6050_TIMEOUT_MS
    );


    if (status != HAL_OK)
    {
        return status;
    }


    // 상위 8비트 + 하위 8비트 → 부호 있는 16비트
    uint16_t raw_x = ((uint16_t)data[0] << 8) | data[1];
    uint16_t raw_y = ((uint16_t)data[2] << 8) | data[3];
    uint16_t raw_z = ((uint16_t)data[4] << 8) | data[5];


    *ax = (int16_t)raw_x;
    *ay = (int16_t)raw_y;
    *az = (int16_t)raw_z;


    return HAL_OK;
}
