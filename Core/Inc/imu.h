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

bool IMU_Init(void);
//bool IMU_GetAccel(AccelData *data);
//bool IMU_GetNewGesture(eIMU_INPUT_DIRECT direction, eIMU_INPUT_DIRECT *gesture);
bool IMU_Update(eIMU_INPUT_DIRECT *gesture);
void IMU_ResetGesture(void);

#endif /* INC_IMU_H_ */
