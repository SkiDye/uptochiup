# 통신 프로토콜

임베디드 시스템에서 필수적인 통신 프로토콜

## 주요 프로토콜

### UART (Universal Asynchronous Receiver/Transmitter)
- **용도**: 시리얼 통신, 디버그 콘솔, GPS, Bluetooth 모듈
- **특징**: 비동기, 전이중, 2선 (TX/RX)
- **속도**: 9600 ~ 921600 bps

### SPI (Serial Peripheral Interface)
- **용도**: 센서, SD 카드, LCD, Flash 메모리
- **특징**: 동기, 전이중, 고속, Master-Slave
- **신호**: MOSI, MISO, SCK, CS

### I2C (Inter-Integrated Circuit)
- **용도**: 센서, EEPROM, RTC, 디스플레이
- **특징**: 동기, 반이중, 2선 (SDA/SCL), 멀티 마스터
- **주소**: 7비트 또는 10비트 슬레이브 주소

### CAN (Controller Area Network)
- **용도**: 자동차, 산업 제어
- **특징**: 차동 신호, 높은 신뢰성, 멀티 마스터
- **속도**: 최대 1 Mbps

## 프로토콜 비교

| 프로토콜 | 속도 | 거리 | 선 개수 | 디바이스 수 | 용도 |
|---------|------|------|---------|------------|------|
| UART | 중간 | 짧음 | 2 | 1:1 | 디버그, 간단한 통신 |
| SPI | 빠름 | 짧음 | 4+ | 1:N | 고속 센서, 메모리 |
| I2C | 느림 | 짧음 | 2 | 1:N | 저속 센서, 주변장치 |
| CAN | 중간 | 김 | 2 | N:N | 자동차, 산업 |

## 학습 로드맵

1. UART로 시작 (가장 간단)
2. SPI (고속 통신)
3. I2C (버스 통신)
4. CAN (고급 프로토콜)
