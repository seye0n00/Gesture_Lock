#include "password.h"

/*
 * 비밀번호:
 * L → R → L → R
 */
static const eIMU_INPUT_DIRECT password[PASSWORD_LENGTH] = {
    eIMU_INPUT_DIRECT_L,
    eIMU_INPUT_DIRECT_R,
    eIMU_INPUT_DIRECT_L,
    eIMU_INPUT_DIRECT_R
};

/* 사용자가 입력한 방향을 저장하는 배열 */
static eIMU_INPUT_DIRECT input_password[PASSWORD_LENGTH];

/* 현재까지 입력된 비밀번호 자리 수 */
static unsigned int input_count = 0;

void Password_ResetInput(void)
{
    unsigned int i;

    input_count = 0;

    for (i = 0; i < PASSWORD_LENGTH; i++) {
        input_password[i] = eIMU_INPUT_DIRECT_Z;
    }
}

bool Password_AddInput(eIMU_INPUT_DIRECT gesture)
{
    // 허용하지 않는 입력
    if (gesture != eIMU_INPUT_DIRECT_L &&
        gesture != eIMU_INPUT_DIRECT_R)
    {
        return false;
    }

    // 비밀번호 입력 개수 초과
    if (input_count >= PASSWORD_LENGTH)
    {
        return false;
    }

    input_password[input_count] = gesture;
    input_count++;

    return true;
}

bool Password_Check(void)
{
    // 비밀번호 4자리 입력 여부 확인
    if (input_count != PASSWORD_LENGTH)
    {
        return false;
    }

    for (unsigned int i = 0; i < PASSWORD_LENGTH; i++)
    {
        if (input_password[i] != password[i])
        {
            return false;
        }
    }

    return true;
}
