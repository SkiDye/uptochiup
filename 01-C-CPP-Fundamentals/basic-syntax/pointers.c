/**
 * 포인터 기초 - 임베디드 필수 개념
 *
 * 포인터는 임베디드에서 레지스터 접근, 메모리 맵 I/O에 필수
 */

#include <stdio.h>
#include <stdint.h>

// 임베디드에서 자주 사용하는 레지스터 접근 매크로
#define REG32(addr) (*(volatile uint32_t *)(addr))

void basic_pointer_example(void) {
    int value = 100;
    int *ptr = &value;  // 포인터 선언 및 초기화

    printf("값: %d\n", value);
    printf("주소: %p\n", (void*)&value);
    printf("포인터가 가리키는 값: %d\n", *ptr);

    *ptr = 200;  // 포인터를 통한 값 변경
    printf("변경된 값: %d\n", value);
}

void pointer_arithmetic(void) {
    uint8_t buffer[5] = {10, 20, 30, 40, 50};
    uint8_t *ptr = buffer;

    printf("\n배열 순회 (포인터 연산):\n");
    for (int i = 0; i < 5; i++) {
        printf("buffer[%d] = %d (주소: %p)\n", i, *(ptr + i), (void*)(ptr + i));
    }
}

void register_access_example(void) {
    // 실제 MCU에서는 이런 식으로 레지스터에 접근
    // 예: STM32의 GPIO 레지스터
    // #define GPIOA_BASE 0x40020000
    // #define GPIOA_ODR  REG32(GPIOA_BASE + 0x14)

    printf("\n레지스터 접근 시뮬레이션:\n");
    uint32_t simulated_register = 0x00000000;
    volatile uint32_t *reg_ptr = &simulated_register;

    // 비트 설정 (set bit 3)
    *reg_ptr |= (1 << 3);
    printf("비트 3 설정 후: 0x%08X\n", *reg_ptr);

    // 비트 클리어 (clear bit 3)
    *reg_ptr &= ~(1 << 3);
    printf("비트 3 클리어 후: 0x%08X\n", *reg_ptr);
}

void function_pointer_example(void) {
    // 함수 포인터 - 상태 머신, 콜백에 사용
    void (*operation)(void);

    operation = basic_pointer_example;
    printf("\n함수 포인터 실행:\n");
    // operation();  // 함수 호출
}

int main(void) {
    printf("=== 임베디드 C 포인터 학습 ===\n\n");

    basic_pointer_example();
    pointer_arithmetic();
    register_access_example();
    function_pointer_example();

    return 0;
}
