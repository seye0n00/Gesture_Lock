## Overview

**Gesture Lock System**

가속도 센서의 좌우 기울기 패턴을 비밀번호로 사용하여 잠금 상태를 제어하는 시스템입니다.

기존 숫자 키패드 방식은 반복적으로 누르는 버튼에 지문 흔적이 남아 비밀번호가 추측될 가능성이 있습니다.

이를 줄이기 위해 숫자 버튼 대신 가속도 센서의 좌우 기울기로 비밀번호를 입력하고, 입력된 패턴을 검증하여 잠금 상태를 제어하는 시스템을 구현했습니다.

**Key Features**

- 좌우 기울기 패턴을 이용한 비밀번호 입력 및 검증
- FSM 기반 중립 상태 확인 및 제스처 중복 인식 방지
- 비밀번호 검증 결과에 따른 잠금 상태 전환
- LCD, LED, 부저를 활용한 입력 과정 및 인증 결과 피드백


## System Architecture

![Gesture Lock System Architecture](docs/gesture_lock_architecture.drawio.svg)


## Component Responsibilities

| 구성요소 | 역할 |
|---|---|
| STM32 (NUCLEO-F446RE) | 센서 데이터 처리, 비밀번호 검증 및 시스템 상태 관리 |
| MPU6050 | 3축 가속도 데이터 측정 |
| Push Button | 사용자 입력 이벤트 발생 |
| LCD 1602 (I2C) | 문자 및 상태 메시지 출력 |
| RGB LED | 잠금 상태에 따른 LED 색상 출력 |
| Passive Buzzer | 입력 및 인증 결과에 따른 소리 출력 (진동 대체) |

## Demo Video

[![Gesture Lock Demo](https://img.youtube.com/vi/KzIor9QrntI/hqdefault.jpg)](https://youtu.be/KzIor9QrntI)
