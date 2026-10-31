#include "mpu6050_i2c.h"

extern I2C_HandleTypeDef hi2c1;

// MPU6050 I2C 주소 (AD0 = LOW)
#define MPU6050_ADDR              (0x68 << 1)


// MPU6050 레지스터 주소
#define MPU6050_WHO_AM_I          0x75
#define MPU6050_PWR_MGMT_1        0x6B
#define MPU6050_PWR_MGMT_2        0x6C
#define MPU6050_ACCEL_CONFIG      0x1C
#define MPU6050_CONFIG            0x1A
#define MPU6050_SMPLRT_DIV        0x19


// MPU6050 설정값
#define MPU6050_DEVICE_ID         0x68
#define MPU6050_PWR_MGMT_1_CFG    0x01
#define MPU6050_PWR_MGMT_2_CFG    0x00
#define MPU6050_ACCEL_CFG         0x00
#define MPU6050_DLPF_CFG          0x04
#define MPU6050_SAMPLE_RATE_CFG   0x09

// I2C 통신 타임아웃 (ms)
#define MPU6050_TIMEOUT_MS        100

// X축 가속도 데이터 시작 레지스터 주소
#define MPU6050_ACCEL_XOUT_H  0x3B

HAL_StatusTypeDef MPU6050_Init(void)
{
    HAL_StatusTypeDef status;
    uint8_t data;

    // 장치 ID 확인
    status = HAL_I2C_Mem_Read(
        &hi2c1,
        MPU6050_ADDR,
        MPU6050_WHO_AM_I,
        I2C_MEMADD_SIZE_8BIT,
        &data,
        1,
        MPU6050_TIMEOUT_MS
    );

    if (status != HAL_OK)
        return status;

    if (data != MPU6050_DEVICE_ID)
        return HAL_ERROR;


    // MPU6050 절전 모드 해제 및 클록 소스 설정
    data = MPU6050_PWR_MGMT_1_CFG;
    status = HAL_I2C_Mem_Write(
        &hi2c1,
        MPU6050_ADDR,
        MPU6050_PWR_MGMT_1,
        I2C_MEMADD_SIZE_8BIT,
        &data,
        1,
        MPU6050_TIMEOUT_MS
    );

    if (status != HAL_OK)
        return status;

    HAL_Delay(10);


    // 가속도계 및 자이로스코프의 모든 축 활성화
    data = MPU6050_PWR_MGMT_2_CFG;
    status = HAL_I2C_Mem_Write(
        &hi2c1,
        MPU6050_ADDR,
        MPU6050_PWR_MGMT_2,
        I2C_MEMADD_SIZE_8BIT,
        &data,
        1,
        MPU6050_TIMEOUT_MS
    );

    if (status != HAL_OK)
        return status;

    // 가속도 측정 범위 설정: ±2g
    data = MPU6050_ACCEL_CFG;
    status = HAL_I2C_Mem_Write(
        &hi2c1,
        MPU6050_ADDR,
        MPU6050_ACCEL_CONFIG,
        I2C_MEMADD_SIZE_8BIT,
        &data,
        1,
        MPU6050_TIMEOUT_MS
    );

    if (status != HAL_OK)
        return status;

    // 디지털 저역 통과 필터 설정: 21Hz
    data = MPU6050_DLPF_CFG;
    status = HAL_I2C_Mem_Write(
        &hi2c1,
        MPU6050_ADDR,
        MPU6050_CONFIG,
        I2C_MEMADD_SIZE_8BIT,
        &data,
        1,
        MPU6050_TIMEOUT_MS
    );

    if (status != HAL_OK)
        return status;


    // 샘플링 주파수 설정: 100Hz
    data = MPU6050_SAMPLE_RATE_CFG;
    status = HAL_I2C_Mem_Write(
        &hi2c1,
        MPU6050_ADDR,
        MPU6050_SMPLRT_DIV,
        I2C_MEMADD_SIZE_8BIT,
        &data,
        1,
        MPU6050_TIMEOUT_MS
    );

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

    // X, Y, Z축 가속도 데이터 6바이트 읽기
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
