#include "imu.h"
#include "mpu6050_i2c.h"
#include <stddef.h>
#include <stdio.h>

#include <math.h>

extern UART_HandleTypeDef huart2;

// MPU6050 ±2g: 16384 LSB/g
#define IMU_LR_THRESHOLD 11585
#define IMU_Z_THRESHOLD  4240
//#define IMU_Z_HOLD_MS 100U


#define IMU_ACCEL_SCALE 16384.0f
#define RAD_TO_DEG      57.2957795f
#define IMU_TEST_DURATION_MS 10000U

static bool test_started = false;
static bool test_finished = false;

static uint32_t test_start_tick = 0;
static uint32_t direction_change_count = 0;
static uint32_t gesture_event_count = 0;


static eIMU_INPUT_DIRECT prev_direction = eIMU_INPUT_DIRECT_MAX;

static eIMU_INPUT_DIRECT cur_input = eIMU_INPUT_DIRECT_Z;
//static uint32_t Z_start = 0;
static bool Z_set = false;

// debug
static const char *imu_direction_str(eIMU_INPUT_DIRECT direction)
{
    switch (direction)
    {
        case eIMU_INPUT_DIRECT_L:
            return "<-";

        case eIMU_INPUT_DIRECT_R:
            return "->";

        case eIMU_INPUT_DIRECT_Z:
            return "Z";

        default:
            return "?";
    }
}

static void imu_print_debug(int16_t x,
                            eIMU_INPUT_DIRECT direction,
                            bool gesture_detected,
                            eIMU_INPUT_DIRECT gesture,
                            uint32_t elapsed_ms)
{
    char buf[140];

    float x_g = (float)x / IMU_ACCEL_SCALE;

    if (x_g > 1.0f)
        x_g = 1.0f;
    else if (x_g < -1.0f)
        x_g = -1.0f;

    float angle = asinf(x_g) * RAD_TO_DEG;

    const char *dir = imu_direction_str(direction);
    const char *event = "";

    if (gesture_detected)
    {
        event = imu_direction_str(gesture);
    }

    int len = snprintf(
        buf,
        sizeof(buf),
        "[%5lu ms] X=%6d | Angle=%6.1f | Dir=%-2s | Event=%s\r\n",
        (unsigned long)elapsed_ms,
        (int)x,
        (double)angle,
        dir,
        event
    );

    if (len > 0 && len < (int)sizeof(buf))
    {
        HAL_UART_Transmit(
            &huart2,
            (uint8_t *)buf,
            (uint16_t)len,
            100
        );
    }
}

static void imu_print_test_result(uint32_t elapsed_ms)
{
    char buf[160];

    int len = snprintf(
        buf,
        sizeof(buf),
        "\r\n[TEST END]\r\n"
        "Duration = %lu ms\r\n"
        "Direction Changes = %lu\r\n"
        "Gesture Events = %lu\r\n",
        (unsigned long)elapsed_ms,
        (unsigned long)direction_change_count,
        (unsigned long)gesture_event_count
    );

    if (len > 0 && len < (int)sizeof(buf))
    {
        HAL_UART_Transmit(
            &huart2,
            (uint8_t *)buf,
            (uint16_t)len,
            100
        );
    }
}

bool imu_init(void)
{
    cur_input = eIMU_INPUT_DIRECT_Z;
    // Z_start=0;
    Z_set=false;

    return (MPU6050_Init() == HAL_OK);
}

void imu_reset_gesture(void)
{
    cur_input = eIMU_INPUT_DIRECT_Z;
    // Z_start=0;
    Z_set=false;
}

bool imu_get_accel(AccelData *data)
{
    if (data == NULL)
    {
        return false;
    }

    return (MPU6050_ReadAccel(&data->x, &data->y, &data->z) == HAL_OK);
}

#if 0
eIMU_INPUT_DIRECT imu_get_direction(const AccelData *data)
{
    if (data->x >= IMU_LR_THRESHOLD)
    {
        return eIMU_INPUT_DIRECT_L;
    }

    if (data->x <= -IMU_LR_THRESHOLD)
    {
        return eIMU_INPUT_DIRECT_R;
    }

    return eIMU_INPUT_DIRECT_Z;
}
#endif

#if 1
// 현재 상태 읽기 (히스테리시스)
eIMU_INPUT_DIRECT imu_get_direction(const AccelData *data)
{
    if (data == NULL)
        return eIMU_INPUT_DIRECT_Z;

    // 이미 오른쪽 제스처를 입력받은 상태
    if (cur_input == eIMU_INPUT_DIRECT_R)
    {
        if (data->x <= -IMU_Z_THRESHOLD)
            return eIMU_INPUT_DIRECT_R;

        return eIMU_INPUT_DIRECT_Z;
    }

    // 이미 왼쪽 제스처를 입력받은 상태
    if (cur_input == eIMU_INPUT_DIRECT_L)
    {
        if (data->x >= IMU_Z_THRESHOLD)
            return eIMU_INPUT_DIRECT_L;

        return eIMU_INPUT_DIRECT_Z;
    }

    // Z 상태에서만 새로운 방향 판정
    if (data->x >= IMU_LR_THRESHOLD)
        return eIMU_INPUT_DIRECT_L;

    if (data->x <= -IMU_LR_THRESHOLD)
        return eIMU_INPUT_DIRECT_R;

    return eIMU_INPUT_DIRECT_Z;
}
#endif



#if 0
static bool imu_check_z_stable(void)
{
    uint32_t now = HAL_GetTick();

    if (Z_start == 0)
    {
        Z_start = now;
        return false;
    }

    if ((uint32_t)(now - Z_start) >= IMU_Z_HOLD_MS)
    {
        Z_start = 0;
        return true;
    }

    return false;
}
#endif

#if 0
bool imu_get_new_gesture(eIMU_INPUT_DIRECT new_input, eIMU_INPUT_DIRECT *gesture)
{
    if (gesture == NULL || new_input >= eIMU_INPUT_DIRECT_MAX)
        return false;

    // Z 방향 유지 처리
    if (new_input == eIMU_INPUT_DIRECT_Z)
    {
        if ((!Z_set || cur_input != eIMU_INPUT_DIRECT_Z)&& imu_check_z_stable())
        {
            Z_set = true;
            cur_input = eIMU_INPUT_DIRECT_Z;
        }
        return false;
    }

    // Z 유지 시간 초기화
    Z_start = 0;

    // 초기 중립 확인 전이거나 이전 제스처가 유지되는 경우
    if (!Z_set || cur_input != eIMU_INPUT_DIRECT_Z)
        return false;

    // 새로운 제스처 입력
    cur_input = new_input;
    *gesture = new_input;

    return true;
}
#endif

#if 1
// 상태 변경 (동작 확인)
bool imu_get_new_gesture(eIMU_INPUT_DIRECT new_input, eIMU_INPUT_DIRECT *gesture)
{
    /*
     * 이전 입력과 현재 입력이 같으면 처리하지 않는다.
     *
     * 예를 들어 IMU가 계속 왼쪽으로 기울어져 있어도
     * L을 여러 번 입력하지 않게 한다.
     */
    if (gesture == NULL || new_input >= eIMU_INPUT_DIRECT_MAX)
    {
        return false;
    }

    /*
     * 중립 상태로 복귀하면
     * 다음 입력을 받을 수 있도록 한다.
     */

    // 초기 중립 확인 전에는 L/R 입력을 무시한다.
    if (!Z_set)
    {
        return false;
    }

    // 중립 상태 확인
    // 초기화 후 처음으로 Z를 감지하면 제스처 입력을 허용한다.
    if (new_input == eIMU_INPUT_DIRECT_Z)
    {
    	//Z_set = true;
        cur_input = eIMU_INPUT_DIRECT_Z;
    	return false;
    }

    /*
     * 중립에서 L 또는 R로 변경된 경우에만
     * 새로운 제스처 이벤트를 발생시킨다.
     */
    // 중립에서 L/R로 변경된 경우에만 이벤트 생성
    if (cur_input == eIMU_INPUT_DIRECT_Z)
    {
        cur_input = new_input;
        *gesture = new_input;
        return true;
    }

    /*
     * 이미 L 또는 R 입력을 받은 상태라면
     * 중립으로 복귀할 때까지 추가 입력을 무시한다.
     */
    return false;
}
#endif


bool imu_update(eIMU_INPUT_DIRECT *gesture)
{
    if (gesture == NULL)
    {
        return false;
    }

    AccelData accel;

    if (!imu_get_accel(&accel))
    {
        return false;
    }

    // 초기 중립 위치 확인
    if (!Z_set)
    {
        if (accel.x >= -IMU_Z_THRESHOLD &&
            accel.x <= IMU_Z_THRESHOLD)
        {
            Z_set = true;
            cur_input = eIMU_INPUT_DIRECT_Z;
        }

        // 초기 중립 확인 루프에서는 이벤트를 발생시키지 않음
        return false;
    }

    eIMU_INPUT_DIRECT direction = imu_get_direction(&accel);


    //return imu_get_new_gesture(direction, gesture);
    // 2. 실제 제스처 이벤트 발생 여부 판단
    bool gesture_detected =
        imu_get_new_gesture(direction, gesture);

    uint32_t now = HAL_GetTick();

    // 측정이 종료되었으면 로그 출력하지 않음
    if (test_finished)
    {
        return gesture_detected;
    }

    // 3. 최초 제스처 발생 시 측정 시작
    if (!test_started)
    {
        if (gesture_detected)
        {
            test_started = true;
            test_start_tick = now;

            direction_change_count = 0;
            gesture_event_count = 1;

            prev_direction = direction;

            const char start_msg[] = "\r\n[TEST START]\r\n";

            HAL_UART_Transmit(
                &huart2,
                (uint8_t *)start_msg,
                sizeof(start_msg) - 1,
                100
            );

            // 최초 이벤트 출력
            imu_print_debug(
                accel.x,
                direction,
                gesture_detected,
                *gesture,
				0U
            );
        }

        return gesture_detected;
    }

    // 4. 측정 시간 확인
    uint32_t elapsed = (uint32_t)(now - test_start_tick);

    if (elapsed >= IMU_TEST_DURATION_MS)
    {
        test_finished = true;
        test_started = false;

        imu_print_test_result(elapsed);

        return gesture_detected;
    }

    // 5. 방향 전환 여부 확인
    bool direction_changed = (direction != prev_direction);

    if (direction_changed)
    {
        direction_change_count++;
        prev_direction = direction;
    }

    // 6. 실제 제스처 이벤트 발생 횟수
    if (gesture_detected)
    {
        gesture_event_count++;
    }

    // 7. 방향 변경 또는 이벤트 발생 시 UART 출력
    if (direction_changed || gesture_detected)
    {
        imu_print_debug(
            accel.x,
            direction,
            gesture_detected,
            gesture_detected ? *gesture : eIMU_INPUT_DIRECT_Z,
            elapsed
        );
    }

    return gesture_detected;
}
