## Overview

**Gesture Lock System**

가속도 센서의 좌우 기울기 패턴을 비밀번호로 사용하여 잠금 상태를 제어하는 시스템입니다.

기존 숫자 키패드 방식은 반복적으로 누르는 버튼에 지문 흔적이 남아 비밀번호가 추측될 가능성이 있습니다.

이를 줄이기 위해 숫자 버튼 대신 가속도 센서의 좌우 기울기로 비밀번호를 입력하고, 입력된 패턴을 검증하여 잠금 상태를 제어하는 시스템을 구현했습니다.

**주요 기능**

- MPU6050 가속도 센서를 활용한 좌우 기울기 인식
- FSM 기반 제스처 입력 및 중복 인식 방지
- 좌우 기울기 패턴을 이용한 비밀번호 입력 및 검증
- 버튼 인터럽트를 통한 비밀번호 검증 및 잠금 상태 전환
- LCD를 통한 비밀번호 입력 과정 및 인증 결과 표시
- RGB LED를 통한 잠금 상태 표시
- PWM 부저를 통한 입력 및 인증 결과 피드백 (진동 대체)


## System Architecture

![Gesture Lock System Architecture](docs/gesture_lock_architecture.drawio.svg)


## Demo Video

[![Gesture Lock Demo](https://img.youtube.com/vi/KzIor9QrntI/hqdefault.jpg)](https://youtu.be/KzIor9QrntI)
