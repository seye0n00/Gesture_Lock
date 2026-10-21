
#include "imu.h"
#include "mpu6050_i2c.h"

#include <stddef.h>
#include <stdio.h>
#include <math.h>

extern UART_HandleTypeDef huart2;

// MPU6050 +/-2g
#define IMU_LR_THRESHOLD       11585
#define IMU_Z_THRESHOLD        4240
#define IMU_Z_HOLD_MS          100U

typedef enum {
    IMU_STATE_WAIT_Z,	// L/R 입력 미허용
    IMU_STATE_READY		// L/R 입력 허용
} eIMU_STATE;

// 히스테리시스로 마지막으로 판별한 방향
static eIMU_INPUT_DIRECT cur_input = eIMU_INPUT_DIRECT_MAX;

// 제스처 입력 허용 상태
static eIMU_STATE imu_state = IMU_STATE_WAIT_Z;

// Z 유지 시간 측정
static uint32_t z_start_tick = 0;
static bool z_timer_running = false;


// Z 유지 시간 측정 상태만 초기화
static void IMU_ResetZTimer(void)
{
    z_start_tick = 0;
    z_timer_running = false;
}


// 제스처 FSM 초기화
void IMU_ResetGesture(void)
{
    cur_input = eIMU_INPUT_DIRECT_MAX;
    imu_state = IMU_STATE_WAIT_Z;

    IMU_ResetZTimer();
}


bool IMU_Init(void)
{
	IMU_ResetGesture();
    return (MPU6050_Init() == HAL_OK);
}

bool IMU_GetAccel(AccelData *data)
{
    if (data == NULL)
        return false;

    return (MPU6050_ReadAccel(&data->x, &data->y, &data->z) == HAL_OK);
}

static eIMU_INPUT_DIRECT IMU_DetectDirection(int16_t x, eIMU_INPUT_DIRECT prev_direction)
{
    if (prev_direction == eIMU_INPUT_DIRECT_L)
    {
        return (x >= IMU_Z_THRESHOLD) ? eIMU_INPUT_DIRECT_L : eIMU_INPUT_DIRECT_Z;
    }

    if (prev_direction == eIMU_INPUT_DIRECT_R)
    {
        return (x <= -IMU_Z_THRESHOLD) ? eIMU_INPUT_DIRECT_R : eIMU_INPUT_DIRECT_Z;
    }

    if (x >= IMU_LR_THRESHOLD)
        return eIMU_INPUT_DIRECT_L;

    if (x <= -IMU_LR_THRESHOLD)
        return eIMU_INPUT_DIRECT_R;

    return eIMU_INPUT_DIRECT_Z;
}

bool IMU_GetNewGesture(eIMU_INPUT_DIRECT direction, eIMU_INPUT_DIRECT *gesture)
{
    if (gesture == NULL || direction >= eIMU_INPUT_DIRECT_MAX)
    {
        return false;
    }

    uint32_t now = HAL_GetTick();

    switch (imu_state)
    {
    case IMU_STATE_WAIT_Z:

        // Z가 아니면 유지 시간 측정 취소
        if (direction != eIMU_INPUT_DIRECT_Z)
        {
        	IMU_ResetZTimer();
            return false;
        }

        // Z 진입 시 타이머 시작
        if (!z_timer_running)
        {
            z_start_tick = now;
            z_timer_running = true;
            return false;
        }

        // Z가 200ms 연속 유지되었는지 확인
        if ((uint32_t)(now - z_start_tick) >= IMU_Z_HOLD_MS)
        {
            imu_state = IMU_STATE_READY;
            IMU_ResetZTimer();
        }

        return false;

    case IMU_STATE_READY:

        // 중립 상태에서는 이벤트 없음
        if (direction == eIMU_INPUT_DIRECT_Z)
            return false;

        // 유효한 LEFT/RIGHT 입력
        *gesture = direction;

        // 다음 입력을 위해 Z 유지 조건부터 재검증
        imu_state = IMU_STATE_WAIT_Z;
        IMU_ResetZTimer();

        return true;

    default:
        // 잘못된 FSM 상태 복구
    	IMU_ResetGesture();
        return false;
    }
}

bool IMU_Update(eIMU_INPUT_DIRECT *gesture)
{
    if (gesture == NULL)
        return false;

    AccelData accel;

    // 센서 데이터 읽기
    if (!IMU_GetAccel(&accel))
    {
        // 읽기 실패 시 연속 Z 유지 조건 무효화
    	IMU_ResetGesture();
        return false;
    }

    // 이전 방향을 이용해 새로운 방향 판별
    eIMU_INPUT_DIRECT direction = IMU_DetectDirection(accel.x, cur_input);

    // 방향 상태 갱신
    cur_input = direction;

    return IMU_GetNewGesture(direction, gesture);
}
