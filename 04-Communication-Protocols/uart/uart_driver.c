/**
 * UART 드라이버 예제
 *
 * 임베디드에서 가장 기본적인 통신 프로토콜
 * - 디버그 출력 (printf)
 * - 센서 모듈 통신 (GPS, Bluetooth 등)
 * - PC 통신
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// UART 레지스터 구조체 (STM32 스타일)
typedef struct {
    volatile uint32_t SR;   // Status register
    volatile uint32_t DR;   // Data register
    volatile uint32_t BRR;  // Baud rate register
    volatile uint32_t CR1;  // Control register 1
    volatile uint32_t CR2;  // Control register 2
    volatile uint32_t CR3;  // Control register 3
} USART_TypeDef;

// UART 설정 구조체
typedef struct {
    uint32_t baudrate;      // 9600, 115200 등
    uint8_t  data_bits;     // 8 또는 9
    uint8_t  stop_bits;     // 1 또는 2
    uint8_t  parity;        // None, Even, Odd
} UART_Config;

// UART 상태
typedef enum {
    UART_OK = 0,
    UART_ERROR,
    UART_BUSY,
    UART_TIMEOUT
} UART_Status;

// 상태 레지스터 비트
#define UART_SR_TXE  (1 << 7)  // Transmit data register empty
#define UART_SR_RXNE (1 << 5)  // Read data register not empty
#define UART_SR_TC   (1 << 6)  // Transmission complete

/**
 * UART 초기화
 */
UART_Status UART_Init(USART_TypeDef *uart, UART_Config *config) {
    // 1. UART 비활성화
    uart->CR1 &= ~(1 << 13);  // UE bit

    // 2. Baud rate 설정
    // BRR = PCLK / baudrate
    // 예: 84MHz / 115200 = 729
    uint32_t pclk = 84000000;  // 시스템 클록
    uart->BRR = pclk / config->baudrate;

    // 3. 데이터 비트 설정
    if (config->data_bits == 9) {
        uart->CR1 |= (1 << 12);  // M bit
    }

    // 4. 스톱 비트 설정
    uart->CR2 &= ~(0x3 << 12);
    if (config->stop_bits == 2) {
        uart->CR2 |= (0x2 << 12);
    }

    // 5. 패리티 설정
    if (config->parity != 0) {
        uart->CR1 |= (1 << 10);  // PCE bit
        if (config->parity == 1) {  // Odd parity
            uart->CR1 |= (1 << 9);
        }
    }

    // 6. TX/RX 활성화
    uart->CR1 |= (1 << 3);  // TE bit (Transmitter enable)
    uart->CR1 |= (1 << 2);  // RE bit (Receiver enable)

    // 7. UART 활성화
    uart->CR1 |= (1 << 13);  // UE bit

    printf("UART 초기화: %lu baud, %d data bits, %d stop bits\n",
           config->baudrate, config->data_bits, config->stop_bits);

    return UART_OK;
}

/**
 * 1바이트 전송 (폴링 방식)
 */
UART_Status UART_SendByte(USART_TypeDef *uart, uint8_t data) {
    uint32_t timeout = 10000;

    // TXE 플래그 대기 (송신 버퍼가 비어있는지)
    while (!(uart->SR & UART_SR_TXE)) {
        if (--timeout == 0) {
            return UART_TIMEOUT;
        }
    }

    // 데이터 전송
    uart->DR = data;

    return UART_OK;
}

/**
 * 1바이트 수신 (폴링 방식)
 */
UART_Status UART_ReceiveByte(USART_TypeDef *uart, uint8_t *data) {
    uint32_t timeout = 10000;

    // RXNE 플래그 대기 (수신 버퍼에 데이터 있는지)
    while (!(uart->SR & UART_SR_RXNE)) {
        if (--timeout == 0) {
            return UART_TIMEOUT;
        }
    }

    // 데이터 읽기
    *data = (uint8_t)(uart->DR & 0xFF);

    return UART_OK;
}

/**
 * 문자열 전송
 */
UART_Status UART_SendString(USART_TypeDef *uart, const char *str) {
    while (*str) {
        if (UART_SendByte(uart, *str++) != UART_OK) {
            return UART_ERROR;
        }
    }
    return UART_OK;
}

/**
 * DMA를 이용한 전송 (고급)
 */
UART_Status UART_SendDMA(USART_TypeDef *uart, uint8_t *data, uint16_t length) {
    // DMA 채널 설정
    // - 메모리 주소: data
    // - 주변장치 주소: &uart->DR
    // - 전송 길이: length

    printf("DMA 전송 시작: %d 바이트\n", length);

    // DMA 전송 시작
    // DMA_Channel->CCR |= DMA_CCR_EN;

    // 인터럽트 방식이므로 즉시 리턴
    return UART_OK;
}

/**
 * DMA 전송 완료 콜백
 */
void UART_DMA_TxCpltCallback(void) {
    printf("DMA 전송 완료\n");
}

/**
 * UART 인터럽트 핸들러
 */
void USART1_IRQHandler(USART_TypeDef *uart) {
    // RXNE 인터럽트 (데이터 수신)
    if (uart->SR & UART_SR_RXNE) {
        uint8_t data = (uint8_t)(uart->DR & 0xFF);

        printf("[IRQ] 수신: 0x%02X ('%c')\n", data, data);

        // 수신 버퍼에 저장 (환형 버퍼 사용 권장)
        // RxBuffer[RxHead++] = data;
    }

    // TC 인터럽트 (전송 완료)
    if (uart->SR & UART_SR_TC) {
        printf("[IRQ] 전송 완료\n");
        uart->SR &= ~UART_SR_TC;  // 플래그 클리어
    }
}

/**
 * printf 리디렉션 (UART로 출력)
 */
int _write(int file, char *ptr, int len) {
    // USART_TypeDef *uart = USART1;

    for (int i = 0; i < len; i++) {
        // UART_SendByte(uart, ptr[i]);
    }

    return len;
}

/**
 * 사용 예제
 */
int main(void) {
    printf("=== UART 드라이버 예제 ===\n\n");

    // 시뮬레이션용 UART 레지스터
    USART_TypeDef uart_sim = {0};
    USART_TypeDef *uart = &uart_sim;

    // UART 설정
    UART_Config config = {
        .baudrate = 115200,
        .data_bits = 8,
        .stop_bits = 1,
        .parity = 0  // None
    };

    // 초기화
    UART_Init(uart, &config);
    printf("\n");

    // 폴링 방식 전송
    printf("--- 폴링 방식 전송 ---\n");
    uart->SR |= UART_SR_TXE;  // 시뮬레이션: TXE 플래그 설정
    UART_SendString(uart, "Hello UART!\r\n");
    printf("전송 완료\n\n");

    // 인터럽트 방식 수신
    printf("--- 인터럽트 방식 수신 ---\n");
    uart->SR |= UART_SR_RXNE;  // 시뮬레이션: 데이터 수신
    uart->DR = 'A';
    USART1_IRQHandler(uart);
    printf("\n");

    // DMA 방식 전송
    printf("--- DMA 방식 전송 ---\n");
    uint8_t dma_data[] = "DMA Transfer Test";
    UART_SendDMA(uart, dma_data, sizeof(dma_data) - 1);
    UART_DMA_TxCpltCallback();  // 시뮬레이션

    return 0;
}

/**
 * UART 전송 방식 비교
 *
 * 1. 폴링 (Polling):
 *    - 간단, CPU 낭비
 *    - 짧은 데이터에 적합
 *
 * 2. 인터럽트 (Interrupt):
 *    - 효율적, 응답성 좋음
 *    - 중간 길이 데이터
 *
 * 3. DMA (Direct Memory Access):
 *    - 최고 효율, CPU 부하 최소
 *    - 긴 데이터 전송
 *
 * 면접 포인트:
 * - Baud rate 계산 방법
 * - 오버런 에러 처리
 * - 환형 버퍼 사용 이유
 * - DMA vs 인터럽트 선택 기준
 */
