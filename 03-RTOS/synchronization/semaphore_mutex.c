/**
 * Semaphore와 Mutex 예제
 *
 * 임베디드 RTOS의 핵심 동기화 메커니즘
 * - Semaphore: 리소스 카운팅, 이벤트 신호
 * - Mutex: 상호 배제, 공유 리소스 보호
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// FreeRTOS API 시뮬레이션
typedef void* SemaphoreHandle_t;
typedef void* QueueHandle_t;

#define pdTRUE  1
#define pdFALSE 0
#define portMAX_DELAY 0xFFFFFFFF

/**
 * 1. Binary Semaphore 예제
 *
 * 용도: 이벤트 신호, 태스크 간 동기화
 * 예: 인터럽트 → 태스크 알림
 */

// Binary Semaphore 핸들
SemaphoreHandle_t xBinarySemaphore;

// 인터럽트 서비스 루틴
void UART_IRQHandler(void) {
    // 데이터 수신 완료

    // 세마포어 Give (ISR에서)
    // BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    // xSemaphoreGiveFromISR(xBinarySemaphore, &xHigherPriorityTaskWoken);
    // portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

    printf("[ISR] 데이터 수신 완료, 세마포어 Give\n");
}

// 데이터 처리 태스크
void vDataProcessTask(void *pvParameters) {
    while (1) {
        // 세마포어 Take (대기)
        // if (xSemaphoreTake(xBinarySemaphore, portMAX_DELAY) == pdTRUE) {
        printf("[Data Task] 세마포어 획득, 데이터 처리 시작\n");

        // 데이터 처리
        printf("[Data Task] 데이터 처리 중...\n");

        printf("[Data Task] 처리 완료\n");
        // }
    }
}

/**
 * 2. Counting Semaphore 예제
 *
 * 용도: 리소스 카운팅
 * 예: 버퍼 슬롯 관리, 풀링
 */

#define MAX_RESOURCES 3
SemaphoreHandle_t xCountingSemaphore;

void vProducerTask(void *pvParameters) {
    uint32_t item = 0;

    while (1) {
        // 리소스 생성
        item++;
        printf("[Producer] 아이템 %lu 생성\n", item);

        // 세마포어 Give (리소스 추가)
        // xSemaphoreGive(xCountingSemaphore);

        // Delay
        printf("[Producer] 대기...\n\n");
    }
}

void vConsumerTask(void *pvParameters) {
    while (1) {
        // 세마포어 Take (리소스 획득 대기)
        // if (xSemaphoreTake(xCountingSemaphore, pdMS_TO_TICKS(1000)) == pdTRUE) {
        printf("[Consumer] 리소스 획득\n");
        printf("[Consumer] 처리 중...\n");
        printf("[Consumer] 처리 완료\n\n");
        // } else {
        //     printf("[Consumer] 타임아웃, 리소스 없음\n");
        // }
    }
}

/**
 * 3. Mutex 예제
 *
 * 용도: 공유 리소스 보호 (상호 배제)
 * 특징: 우선순위 상속으로 Priority Inversion 방지
 */

SemaphoreHandle_t xMutex;

// 공유 리소스 (UART)
typedef struct {
    char buffer[128];
    bool is_busy;
} UART_Handle;

UART_Handle uart;

void UART_Send(const char *message) {
    // Mutex 획득 (공유 리소스 잠금)
    // if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
    printf("    [UART] Mutex 획득\n");

    // 크리티컬 섹션 시작
    uart.is_busy = true;
    printf("    [UART] 전송: %s\n", message);

    // 전송 시뮬레이션
    // HAL_UART_Transmit(&huart1, message, strlen(message), 100);

    uart.is_busy = false;
    // 크리티컬 섹션 종료

    // Mutex 해제
    // xSemaphoreGive(xMutex);
    printf("    [UART] Mutex 해제\n");
    // }
}

void vTask1(void *pvParameters) {
    while (1) {
        printf("[Task 1] UART 전송 시도\n");
        UART_Send("Task 1 Message");
        printf("[Task 1] 완료\n\n");

        // vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void vTask2(void *pvParameters) {
    while (1) {
        printf("[Task 2] UART 전송 시도\n");
        UART_Send("Task 2 Message");
        printf("[Task 2] 완료\n\n");

        // vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/**
 * 4. 우선순위 역전 (Priority Inversion) 문제
 */
void demonstrate_priority_inversion(void) {
    printf("=== 우선순위 역전 (Priority Inversion) ===\n\n");

    printf("상황:\n");
    printf("- Task Low (우선순위 1): Mutex 획득\n");
    printf("- Task High (우선순위 3): Mutex 대기\n");
    printf("- Task Medium (우선순위 2): 실행 중\n\n");

    printf("문제:\n");
    printf("Task High가 Task Medium보다 우선순위가 높지만,\n");
    printf("Task Low가 Mutex를 쥐고 있어서 대기해야 함\n\n");

    printf("해결책: Mutex의 우선순위 상속 (Priority Inheritance)\n");
    printf("- Task Low가 일시적으로 Task High의 우선순위를 상속\n");
    printf("- Task Low가 빨리 실행되어 Mutex 해제\n");
    printf("- Task High가 Mutex 획득하여 실행\n\n");
}

/**
 * 메인 함수
 */
int main(void) {
    printf("=== Semaphore & Mutex 예제 ===\n\n");

    // Binary Semaphore 생성
    // xBinarySemaphore = xSemaphoreCreateBinary();
    printf("✓ Binary Semaphore 생성\n");

    // Counting Semaphore 생성
    // xCountingSemaphore = xSemaphoreCreateCounting(MAX_RESOURCES, 0);
    printf("✓ Counting Semaphore 생성 (최대: %d)\n", MAX_RESOURCES);

    // Mutex 생성
    // xMutex = xSemaphoreCreateMutex();
    printf("✓ Mutex 생성\n\n");

    printf("--- 사용 예제 ---\n\n");

    printf("1. Binary Semaphore: ISR → Task 동기화\n");
    UART_IRQHandler();  // 시뮬레이션
    printf("\n");

    printf("2. Mutex: 공유 리소스 보호\n");
    UART_Send("Test Message");
    printf("\n");

    demonstrate_priority_inversion();

    return 0;
}

/**
 * Semaphore vs Mutex 비교
 *
 * Semaphore:
 * - 카운터 기반 (0 이상)
 * - 신호 전달 (ISR → Task)
 * - 리소스 카운팅
 * - 누구나 Give 가능
 *
 * Mutex:
 * - Binary (0 또는 1)
 * - 상호 배제 (Mutual Exclusion)
 * - 우선순위 상속
 * - 획득한 태스크만 해제 가능
 *
 * 선택 기준:
 * - 이벤트 신호 → Binary Semaphore
 * - 리소스 카운팅 → Counting Semaphore
 * - 공유 리소스 보호 → Mutex
 */
