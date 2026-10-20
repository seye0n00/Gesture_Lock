#ifndef INC_IMU_H_
#define INC_IMU_H_

#include <stdint.h>
#include <stdbool.h>
#include "system_types.h"

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} AccelData;

bool imu_init(void);
bool imu_get_accel(AccelData *data);
bool imu_get_new_gesture(eIMU_INPUT_DIRECT direction, eIMU_INPUT_DIRECT *gesture);
bool imu_update(eIMU_INPUT_DIRECT *gesture);
void imu_reset_gesture(void);

eIMU_INPUT_DIRECT imu_get_direction(const AccelData *data);

#endif /* INC_IMU_H_ */
