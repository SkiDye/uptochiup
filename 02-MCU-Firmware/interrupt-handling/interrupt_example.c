/**
 * 인터럽트 처리 예제
 *
 * 임베디드 시스템의 핵심 개념
 * - 외부 이벤트에 즉각 반응
 * - 폴링(Polling)보다 효율적
 * - 실시간 응답성 향상
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// 인터럽트 우선순위
#define IRQ_PRIORITY_HIGH    0
#define IRQ_PRIORITY_MEDIUM  1
#define IRQ_PRIORITY_LOW     2

// 전역 변수 (인터럽트와 메인 간 공유)
volatile uint32_t button_press_count = 0;
volatile bool button_pressed_flag = false;
volatile uint32_t timer_tick_count = 0;

/**
 * 외부 인터럽트 서비스 루틴 (ISR)
 *
 * 주의사항:
 * 1. ISR은 가능한 짧고 빠르게
 * 2. 플래그만 설정하고 실제 처리는 메인 루프에서
 * 3. volatile 변수 사용
 * 4. printf 같은 무거운 함수 피하기
 */
void EXTI0_IRQHandler(void) {
    // 버튼 눌림 감지 (외부 인터럽트)

    // 카운터 증가
    button_press_count++;

    // 플래그 설정 (메인 루프에서 처리)
    button_pressed_flag = true;

    // 인터럽트 플래그 클리어
    // (실제 MCU에서는 필수)
    // EXTI->PR = (1 << 0);
}

/**
 * 타이머 인터럽트 서비스 루틴
 *
 * 주기적으로 발생하는 인터럽트
 * 예: 1ms마다 시스템 틱 업데이트
 */
void TIM2_IRQHandler(void) {
    // 타이머 틱 카운터 증가
    timer_tick_count++;

    // 타이머 인터럽트 플래그 클리어
    // (실제 MCU에서는 필수)
    // TIM2->SR &= ~TIM_SR_UIF;
}

/**
 * 인터럽트 우선순위 설정
 */
void IRQ_SetPriority(uint8_t irq_number, uint8_t priority) {
    // NVIC (Nested Vectored Interrupt Controller) 설정
    // 실제 코드:
    // NVIC_SetPriority(irq_number, priority);

    printf("IRQ %d 우선순위를 %d로 설정\n", irq_number, priority);
}

/**
 * 인터럽트 활성화
 */
void IRQ_Enable(uint8_t irq_number) {
    // 실제 코드:
    // NVIC_EnableIRQ(irq_number);

    printf("IRQ %d 활성화\n", irq_number);
}

/**
 * 크리티컬 섹션 (Critical Section)
 *
 * 인터럽트 비활성화 → 작업 수행 → 인터럽트 재활성화
 * 데이터 무결성 보장
 */
uint32_t get_button_count_safe(void) {
    uint32_t count;

    // 인터럽트 비활성화
    __disable_irq();

    // 공유 변수 읽기
    count = button_press_count;

    // 인터럽트 재활성화
    __enable_irq();

    return count;
}

/**
 * 디바운싱 (Debouncing)
 *
 * 버튼 누를 때 기계적 떨림 제거
 */
#define DEBOUNCE_TIME_MS 50
static uint32_t last_button_time = 0;

bool is_button_debounced(uint32_t current_time) {
    if (current_time - last_button_time > DEBOUNCE_TIME_MS) {
        last_button_time = current_time;
        return true;
    }
    return false;
}

/**
 * 메인 함수 - 인터럽트 기반 시스템
 */
int main(void) {
    printf("=== 인터럽트 처리 예제 ===\n\n");

    // 인터럽트 우선순위 설정
    printf("인터럽트 초기화\n");
    IRQ_SetPriority(6, IRQ_PRIORITY_HIGH);    // EXTI0 (버튼)
    IRQ_SetPriority(28, IRQ_PRIORITY_MEDIUM); // TIM2 (타이머)

    // 인터럽트 활성화
    IRQ_Enable(6);   // EXTI0
    IRQ_Enable(28);  // TIM2

    printf("\n시스템 시작 (인터럽트 대기 중...)\n");
    printf("'q' 입력 시 종료\n\n");

    // 메인 루프 (폴링 없이 인터럽트 기반)
    while (1) {
        // 버튼 눌림 플래그 확인
        if (button_pressed_flag) {
            button_pressed_flag = false;  // 플래그 클리어

            // 디바운싱 처리
            if (is_button_debounced(timer_tick_count)) {
                printf("[%lu ms] 버튼 눌림 감지! (총 %lu회)\n",
                       timer_tick_count, button_press_count);
            }
        }

        // 1초마다 상태 출력
        static uint32_t last_print_time = 0;
        if (timer_tick_count - last_print_time >= 1000) {
            last_print_time = timer_tick_count;
            printf("[상태] 시스템 시간: %lu초, 버튼 누름: %lu회\n",
                   timer_tick_count / 1000,
                   get_button_count_safe());
        }

        // 실제 MCU에서는 저전력 모드 진입
        // __WFI();  // Wait For Interrupt
    }

    return 0;
}

/**
 * 인터럽트 모범 사례
 *
 * 1. ISR은 최대한 짧게 (< 10 마이크로초)
 * 2. ISR에서 플래그만 설정, 처리는 메인에서
 * 3. volatile 키워드로 공유 변수 선언
 * 4. 크리티컬 섹션에서 인터럽트 비활성화
 * 5. 우선순위 신중하게 설정
 * 6. ISR에서 무거운 함수 호출 금지
 * 7. 디바운싱 구현
 */
