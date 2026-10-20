#ifndef INC_LOCKDISP_H_
#define INC_LOCKDISP_H_

#include <stddef.h>
#include <stdbool.h>

// LCD 잠금 화면 초기화
bool lockdisp_init(void);

// 잠금 상태 표시 갱신
void lockdisp_update_lock(void);

// 비밀번호 입력 상태 표시 갱신
void lockdisp_update_input(void);

// 입력된 제스처 문자열 표시
void lockdisp_next_input(const char *str, size_t len);

#endif /* INC_LOCKDISP_H_ */
