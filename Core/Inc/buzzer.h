#ifndef INC_BUZZER_H_
#define INC_BUZZER_H_

#include <stdbool.h>
#include "main.h"
#include "system_types.h"

void Buzzer_Stop(void);
void Buzzer_Init(void);
void Buzzer_On(eINPUT_STATE state);
void Buzzer_Update(void);

#endif /* INC_BUZZER_H_ */
