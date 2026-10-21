#ifndef INC_PASSWORD_H_
#define INC_PASSWORD_H_

#include <stdbool.h>
#include "system_types.h"

#define PASSWORD_LENGTH 4

void Password_ResetInput(void);
bool Password_AddInput(eIMU_INPUT_DIRECT gesture);
bool Password_Check(void);

#endif /* INC_PASSWORD_H_ */
