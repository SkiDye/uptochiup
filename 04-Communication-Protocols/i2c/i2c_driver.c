/**
 * I2C 드라이버 예제
 *
 * I2C (Inter-Integrated Circuit)
 * - 2선 버스 (SDA, SCL)
 * - 마스터-슬레이브 구조
 * - 다중 디바이스 지원 (주소로 구분)
 * - 센서, EEPROM, RTC 등에 사용
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// I2C 레지스터 구조체
typedef struct {
    volatile uint32_t CR1;      // Control register 1
    volatile uint32_t CR2;      // Control register 2
    volatile uint32_t OAR1;     // Own address register 1
    volatile uint32_t OAR2;     // Own address register 2
    volatile uint32_t DR;       // Data register
    volatile uint32_t SR1;      // Status register 1
    volatile uint32_t SR2;      // Status register 2
    volatile uint32_t CCR;      // Clock control register
    volatile uint32_t TRISE;    // TRISE register
} I2C_TypeDef;

// I2C 상태
typedef enum {
    I2C_OK = 0,
    I2C_ERROR,
    I2C_BUSY,
    I2C_TIMEOUT,
    I2C_NACK
} I2C_Status;

// I2C 속도
typedef enum {
    I2C_SPEED_STANDARD = 100000,   // 100 kHz
    I2C_SPEED_FAST = 400000,       // 400 kHz
    I2C_SPEED_FAST_PLUS = 1000000  // 1 MHz
} I2C_Speed;

// 상태 레지스터 비트
#define I2C_SR1_SB      (1 << 0)   // Start bit
#define I2C_SR1_ADDR    (1 << 1)   // Address sent
#define I2C_SR1_BTF     (1 << 2)   // Byte transfer finished
#define I2C_SR1_TXE     (1 << 7)   // Data register empty
#define I2C_SR1_RXNE    (1 << 6)   // Data register not empty

/**
 * I2C 초기화
 */
I2C_Status I2C_Init(I2C_TypeDef *i2c, I2C_Speed speed) {
    // 1. I2C 비활성화
    i2c->CR1 &= ~(1 << 0);  // PE bit

    // 2. 클록 속도 설정
    uint32_t pclk = 42000000;  // APB1 클록 (42MHz)
    uint32_t freq = pclk / 1000000;  // MHz 단위

    i2c->CR2 = freq;

    // 3. CCR 계산 (Clock Control Register)
    uint32_t ccr;
    if (speed <= I2C_SPEED_STANDARD) {
        // Standard mode (100 kHz)
        ccr = pclk / (speed * 2);
    } else {
        // Fast mode (400 kHz)
        ccr = pclk / (speed * 3);
        i2c->CCR |= (1 << 15);  // F/S bit
    }
    i2c->CCR |= ccr;

    // 4. Rise time 설정
    i2c->TRISE = freq + 1;

    // 5. I2C 활성화
    i2c->CR1 |= (1 << 0);  // PE bit

    printf("I2C 초기화: %lu Hz\n", speed);

    return I2C_OK;
}

/**
 * START 조건 생성
 */
I2C_Status I2C_Start(I2C_TypeDef *i2c) {
    uint32_t timeout = 10000;

    // START 비트 설정
    i2c->CR1 |= (1 << 8);

    // SB 플래그 대기
    while (!(i2c->SR1 & I2C_SR1_SB)) {
        if (--timeout == 0) {
            return I2C_TIMEOUT;
        }
    }

    printf("  I2C START\n");
    return I2C_OK;
}

/**
 * STOP 조건 생성
 */
void I2C_Stop(I2C_TypeDef *i2c) {
    i2c->CR1 |= (1 << 9);
    printf("  I2C STOP\n");
}

/**
 * 슬레이브 주소 전송
 */
I2C_Status I2C_SendAddress(I2C_TypeDef *i2c, uint8_t address, bool read) {
    uint32_t timeout = 10000;

    // 주소 전송 (R/W 비트 포함)
    if (read) {
        i2c->DR = (address << 1) | 0x01;
    } else {
        i2c->DR = (address << 1) | 0x00;
    }

    // ADDR 플래그 대기
    while (!(i2c->SR1 & I2C_SR1_ADDR)) {
        if (--timeout == 0) {
            return I2C_NACK;  // 슬레이브 응답 없음
        }
    }

    // SR1, SR2 읽기로 ADDR 플래그 클리어
    (void)i2c->SR1;
    (void)i2c->SR2;

    printf("  주소 전송: 0x%02X (%s)\n", address, read ? "READ" : "WRITE");

    return I2C_OK;
}

/**
 * 데이터 쓰기
 */
I2C_Status I2C_WriteData(I2C_TypeDef *i2c, uint8_t data) {
    uint32_t timeout = 10000;

    // TXE 플래그 대기
    while (!(i2c->SR1 & I2C_SR1_TXE)) {
        if (--timeout == 0) {
            return I2C_TIMEOUT;
        }
    }

    // 데이터 전송
    i2c->DR = data;

    printf("  데이터 쓰기: 0x%02X\n", data);

    return I2C_OK;
}

/**
 * 데이터 읽기
 */
I2C_Status I2C_ReadData(I2C_TypeDef *i2c, uint8_t *data, bool ack) {
    uint32_t timeout = 10000;

    // ACK/NACK 설정
    if (ack) {
        i2c->CR1 |= (1 << 10);   // ACK
    } else {
        i2c->CR1 &= ~(1 << 10);  // NACK
    }

    // RXNE 플래그 대기
    while (!(i2c->SR1 & I2C_SR1_RXNE)) {
        if (--timeout == 0) {
            return I2C_TIMEOUT;
        }
    }

    // 데이터 읽기
    *data = (uint8_t)i2c->DR;

    printf("  데이터 읽기: 0x%02X\n", *data);

    return I2C_OK;
}

/**
 * 레지스터 쓰기 (센서에서 자주 사용)
 */
I2C_Status I2C_WriteRegister(I2C_TypeDef *i2c, uint8_t slave_addr,
                             uint8_t reg_addr, uint8_t data) {
    printf("레지스터 쓰기 (주소: 0x%02X, 레지스터: 0x%02X, 값: 0x%02X)\n",
           slave_addr, reg_addr, data);

    // START
    if (I2C_Start(i2c) != I2C_OK) return I2C_ERROR;

    // 슬레이브 주소 전송 (WRITE)
    if (I2C_SendAddress(i2c, slave_addr, false) != I2C_OK) {
        I2C_Stop(i2c);
        return I2C_NACK;
    }

    // 레지스터 주소 전송
    if (I2C_WriteData(i2c, reg_addr) != I2C_OK) {
        I2C_Stop(i2c);
        return I2C_ERROR;
    }

    // 데이터 전송
    if (I2C_WriteData(i2c, data) != I2C_OK) {
        I2C_Stop(i2c);
        return I2C_ERROR;
    }

    // STOP
    I2C_Stop(i2c);

    return I2C_OK;
}

/**
 * 레지스터 읽기 (센서에서 자주 사용)
 */
I2C_Status I2C_ReadRegister(I2C_TypeDef *i2c, uint8_t slave_addr,
                            uint8_t reg_addr, uint8_t *data) {
    printf("레지스터 읽기 (주소: 0x%02X, 레지스터: 0x%02X)\n",
           slave_addr, reg_addr);

    // START
    if (I2C_Start(i2c) != I2C_OK) return I2C_ERROR;

    // 슬레이브 주소 전송 (WRITE)
    if (I2C_SendAddress(i2c, slave_addr, false) != I2C_OK) {
        I2C_Stop(i2c);
        return I2C_NACK;
    }

    // 레지스터 주소 전송
    if (I2C_WriteData(i2c, reg_addr) != I2C_OK) {
        I2C_Stop(i2c);
        return I2C_ERROR;
    }

    // REPEATED START
    if (I2C_Start(i2c) != I2C_OK) return I2C_ERROR;

    // 슬레이브 주소 전송 (READ)
    if (I2C_SendAddress(i2c, slave_addr, true) != I2C_OK) {
        I2C_Stop(i2c);
        return I2C_NACK;
    }

    // 데이터 읽기 (NACK 전송)
    if (I2C_ReadData(i2c, data, false) != I2C_OK) {
        I2C_Stop(i2c);
        return I2C_ERROR;
    }

    // STOP
    I2C_Stop(i2c);

    return I2C_OK;
}

/**
 * 사용 예제 - 센서 초기화 및 읽기
 */
void example_sensor_init(I2C_TypeDef *i2c) {
    printf("\n=== 센서 초기화 예제 (MPU6050 가속도/자이로 센서) ===\n\n");

    uint8_t slave_addr = 0x68;  // MPU6050 주소

    // 1. WHO_AM_I 레지스터 읽기 (0x75)
    uint8_t who_am_i;
    I2C_ReadRegister(i2c, slave_addr, 0x75, &who_am_i);
    printf("→ WHO_AM_I: 0x%02X (예상: 0x68)\n\n", who_am_i);

    // 2. 파워 관리 레지스터 설정 (0x6B)
    I2C_WriteRegister(i2c, slave_addr, 0x6B, 0x00);  // Wake up
    printf("→ 센서 활성화\n\n");

    // 3. 가속도 데이터 읽기 (0x3B~0x40)
    printf("가속도 데이터 읽기:\n");
    uint8_t accel_x_h, accel_x_l;
    I2C_ReadRegister(i2c, slave_addr, 0x3B, &accel_x_h);
    I2C_ReadRegister(i2c, slave_addr, 0x3C, &accel_x_l);

    int16_t accel_x = (accel_x_h << 8) | accel_x_l;
    printf("→ X축 가속도: %d\n", accel_x);
}

/**
 * 메인 함수
 */
int main(void) {
    printf("=== I2C 드라이버 예제 ===\n\n");

    // 시뮬레이션용 I2C 레지스터
    I2C_TypeDef i2c_sim = {0};
    I2C_TypeDef *i2c = &i2c_sim;

    // I2C 초기화
    I2C_Init(i2c, I2C_SPEED_STANDARD);
    printf("\n");

    // 시뮬레이션: 플래그 설정
    i2c->SR1 = I2C_SR1_SB | I2C_SR1_ADDR | I2C_SR1_TXE | I2C_SR1_RXNE;
    i2c->DR = 0x68;  // WHO_AM_I 응답

    // 센서 초기화 및 읽기
    example_sensor_init(i2c);

    return 0;
}

/**
 * I2C 핵심 개념
 *
 * 1. START/STOP 조건
 *    - START: SCL=HIGH에서 SDA=HIGH→LOW
 *    - STOP: SCL=HIGH에서 SDA=LOW→HIGH
 *
 * 2. ACK/NACK
 *    - ACK: 수신 성공 (SDA=LOW)
 *    - NACK: 수신 실패 또는 마지막 바이트 (SDA=HIGH)
 *
 * 3. 주소 지정
 *    - 7비트 주소 + R/W 비트
 *    - 다중 디바이스 지원
 *
 * 4. 클록 스트레칭
 *    - 슬레이브가 SCL을 LOW로 유지하여 마스터 대기
 *
 * 면접 포인트:
 * - I2C vs SPI 차이점
 * - Pull-up 저항 필요 이유
 * - Clock stretching 개념
 * - Multi-master 충돌 처리
 */
