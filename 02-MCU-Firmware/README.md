# MCU 펌웨어 개발

마이크로컨트롤러 펌웨어 프로그래밍 핵심 기술

## 학습 내용

### 1. GPIO 제어 (`gpio-control/`)
- 디지털 입출력
- LED 제어
- 버튼 입력 처리
- Pull-up/Pull-down 저항

### 2. 타이머/PWM (`timer-pwm/`)
- 타이머 인터럽트
- PWM 신호 생성
- 모터 속도 제어
- 서보 모터 제어

### 3. ADC/DAC (`adc-dac/`)
- 아날로그 입력 읽기
- 센서 값 변환
- 아날로그 출력

### 4. 인터럽트 처리 (`interrupt-handling/`)
- 외부 인터럽트
- 타이머 인터럽트
- 우선순위 관리
- ISR 작성 규칙

## 주요 MCU 플랫폼

- **STM32** (ARM Cortex-M): 산업 표준
- **ESP32**: Wi-Fi/Bluetooth 내장
- **Arduino**: 학습용
- **Raspberry Pi Pico**: RP2040

## 개발 도구

- **IDE**: STM32CubeIDE, PlatformIO, Arduino IDE
- **디버거**: ST-Link, J-Link
- **프로그래머**: OpenOCD, STM32CubeProgrammer
