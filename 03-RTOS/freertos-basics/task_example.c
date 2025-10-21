/**
 * FreeRTOS 태스크 예제
 *
 * FreeRTOS는 임베디드에서 가장 많이 사용되는 RTOS
 * - 멀티태스킹 지원
 * - 우선순위 기반 스케줄링
 * - 작은 메모리 풋프린트
 */

// 이 예제는 FreeRTOS API 사용법을 보여주는 샘플 코드입니다
// 실제 실행을 위해서는 FreeRTOS 라이브러리가 필요합니다

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// FreeRTOS 헤더 (실제 프로젝트에서)
// #include "FreeRTOS.h"
// #include "task.h"

// 예제를 위한 타입 정의
typedef void (*TaskFunction_t)(void *);
typedef void* TaskHandle_t;

#define pdMS_TO_TICKS(ms) (ms)
#define portMAX_DELAY (0xFFFFFFFF)

/**
 * LED 깜빡임 태스크
 *
 * 우선순위: 1 (낮음)
 * 주기: 500ms
 */
void vLEDTask(void *pvParameters) {
    const uint32_t delay_ms = 500;

    // 태스크는 무한 루프로 실행
    while (1) {
        printf("[LED Task] LED ON\n");

        // 태스크 지연 (다른 태스크에 CPU 양보)
        // vTaskDelay(pdMS_TO_TICKS(delay_ms));
        printf("[LED Task] Delay %lu ms\n", delay_ms);

        printf("[LED Task] LED OFF\n");
        // vTaskDelay(pdMS_TO_TICKS(delay_ms));
        printf("[LED Task] Delay %lu ms\n", delay_ms);
    }
}

/**
 * 센서 읽기 태스크
 *
 * 우선순위: 2 (중간)
 * 주기: 100ms
 */
void vSensorTask(void *pvParameters) {
    uint32_t sensor_value = 0;
    const uint32_t delay_ms = 100;

    while (1) {
        // 센서 값 읽기 (시뮬레이션)
        sensor_value = (sensor_value + 1) % 100;

        printf("[Sensor Task] 센서 값: %lu\n", sensor_value);

        // 임계값 확인
        if (sensor_value > 80) {
            printf("[Sensor Task] 경고! 센서 값이 임계값 초과\n");
        }

        // vTaskDelay(pdMS_TO_TICKS(delay_ms));
        printf("[Sensor Task] Delay %lu ms\n", delay_ms);
    }
}

/**
 * 통신 태스크
 *
 * 우선순위: 3 (높음)
 * 이벤트 기반 실행
 */
void vCommTask(void *pvParameters) {
    while (1) {
        // 데이터 수신 대기
        printf("[Comm Task] 데이터 수신 대기...\n");

        // 실제로는 큐나 세마포어 대기
        // xQueueReceive(uart_queue, &data, portMAX_DELAY);

        printf("[Comm Task] 데이터 수신: 'Hello'\n");
        printf("[Comm Task] 데이터 처리 중...\n");

        // 처리 완료 후 다시 대기
    }
}

/**
 * Idle 태스크 훅
 *
 * CPU가 유휴 상태일 때 호출
 * 저전력 모드 진입에 사용
 */
void vApplicationIdleHook(void) {
    // 저전력 모드 진입
    // __WFI();  // Wait For Interrupt
}

/**
 * 스택 오버플로우 훅
 *
 * 태스크 스택 오버플로우 감지 시 호출
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    printf("스택 오버플로우 감지: %s\n", pcTaskName);

    // 오류 처리
    while (1) {
        // 시스템 정지
    }
}

/**
 * 메인 함수 - 태스크 생성 및 스케줄러 시작
 */
int main(void) {
    printf("=== FreeRTOS 태스크 예제 ===\n\n");

    // 태스크 핸들
    // TaskHandle_t led_task_handle = NULL;
    // TaskHandle_t sensor_task_handle = NULL;
    // TaskHandle_t comm_task_handle = NULL;

    printf("태스크 생성 중...\n\n");

    // LED 태스크 생성
    // xTaskCreate(
    //     vLEDTask,           // 태스크 함수
    //     "LED Task",         // 태스크 이름
    //     128,                // 스택 크기 (words)
    //     NULL,               // 파라미터
    //     1,                  // 우선순위 (낮음)
    //     &led_task_handle    // 태스크 핸들
    // );
    printf("✓ LED Task 생성 (우선순위: 1)\n");

    // 센서 태스크 생성
    // xTaskCreate(vSensorTask, "Sensor Task", 128, NULL, 2, &sensor_task_handle);
    printf("✓ Sensor Task 생성 (우선순위: 2)\n");

    // 통신 태스크 생성
    // xTaskCreate(vCommTask, "Comm Task", 256, NULL, 3, &comm_task_handle);
    printf("✓ Comm Task 생성 (우선순위: 3)\n");

    printf("\n스케줄러 시작\n");
    printf("우선순위: Comm(3) > Sensor(2) > LED(1)\n\n");

    // 스케줄러 시작 (이후 리턴되지 않음)
    // vTaskStartScheduler();

    // 시뮬레이션: 태스크 함수 호출
    printf("--- 태스크 실행 시뮬레이션 ---\n\n");

    // 실제로는 스케줄러가 자동으로 관리
    printf("Tick 0-100ms: Comm Task 실행\n");
    printf("Tick 100ms: Sensor Task 실행\n");
    printf("Tick 500ms: LED Task 실행\n");

    printf("\n여기는 도달하지 않습니다 (스케줄러가 제어)\n");

    return 0;
}

/**
 * FreeRTOS 핵심 개념
 *
 * 1. 태스크: 독립적인 실행 단위
 * 2. 우선순위: 높을수록 먼저 실행
 * 3. 스케줄러: 태스크 전환 관리
 * 4. 블로킹: 대기 중 CPU 양보
 * 5. 컨텍스트 스위칭: 태스크 간 전환
 *
 * 태스크 상태:
 * - Running: 실행 중
 * - Ready: 실행 대기
 * - Blocked: 이벤트/시간 대기
 * - Suspended: 일시 중지
 */
