#ifndef INC_PASSWORD_H_
#define INC_PASSWORD_H_

#include <stdbool.h>
#include "system_types.h"

#define PASSWORD_LENGTH 4

void reset_password_input(void);
bool add_password_input(eIMU_INPUT_DIRECT gesture);
bool check_password(void);

#endif /* INC_PASSWORD_H_ */
