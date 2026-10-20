#ifndef INC_BUZZER_H_
#define INC_BUZZER_H_

#include <stdbool.h>
#include "main.h"
#include "system_types.h"

void buzzer_stop(void);
void buzzer_init(void);
void buzzer_on(eINPUT_STATE state);
void buzzer_update(void);

#endif /* INC_BUZZER_H_ */
