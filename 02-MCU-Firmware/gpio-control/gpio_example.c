/**
 * GPIO 제어 예제 (STM32 스타일)
 *
 * 레지스터 직접 접근 방식으로 GPIO 제어
 * 실제 하드웨어 이해를 위한 베어메탈 프로그래밍
 */

#include <stdint.h>
#include <stdbool.h>

// STM32 GPIO 레지스터 구조체 (예시)
typedef struct {
    volatile uint32_t MODER;    // Mode register
    volatile uint32_t OTYPER;   // Output type register
    volatile uint32_t OSPEEDR;  // Output speed register
    volatile uint32_t PUPDR;    // Pull-up/pull-down register
    volatile uint32_t IDR;      // Input data register
    volatile uint32_t ODR;      // Output data register
    volatile uint32_t BSRR;     // Bit set/reset register
    volatile uint32_t LCKR;     // Configuration lock register
    volatile uint32_t AFR[2];   // Alternate function registers
} GPIO_TypeDef;

// 시뮬레이션을 위한 가상 GPIO 레지스터
#ifdef SIMULATION
static struct {
    uint32_t MODER;
    uint32_t OTYPER;
    uint32_t OSPEEDR;
    uint32_t PUPDR;
    uint32_t IDR;
    uint32_t ODR;
    uint32_t BSRR;
    uint32_t LCKR;
    uint32_t AFR[2];
} GPIOA_SIM, GPIOB_SIM;

#define GPIOA ((GPIO_TypeDef*)&GPIOA_SIM)
#define GPIOB ((GPIO_TypeDef*)&GPIOB_SIM)
#else
// 실제 STM32에서는 이런 주소 사용
// #define GPIOA_BASE 0x40020000
// #define GPIOB_BASE 0x40020400
// #define GPIOA ((GPIO_TypeDef*)GPIOA_BASE)
// #define GPIOB ((GPIO_TypeDef*)GPIOB_BASE)
#endif

// GPIO 모드
typedef enum {
    GPIO_MODE_INPUT = 0,
    GPIO_MODE_OUTPUT = 1,
    GPIO_MODE_ALTERNATE = 2,
    GPIO_MODE_ANALOG = 3
} GPIO_Mode;

// GPIO Pull-up/Pull-down
typedef enum {
    GPIO_PUPD_NONE = 0,
    GPIO_PUPD_PULLUP = 1,
    GPIO_PUPD_PULLDOWN = 2
} GPIO_PuPd;

/**
 * GPIO 핀 모드 설정
 */
void GPIO_SetMode(GPIO_TypeDef *GPIOx, uint8_t pin, GPIO_Mode mode) {
    // 해당 핀의 2비트를 클리어하고 새 모드 설정
    GPIOx->MODER &= ~(0x3 << (pin * 2));
    GPIOx->MODER |= (mode << (pin * 2));
}

/**
 * GPIO Pull-up/Pull-down 설정
 */
void GPIO_SetPuPd(GPIO_TypeDef *GPIOx, uint8_t pin, GPIO_PuPd pupd) {
    GPIOx->PUPDR &= ~(0x3 << (pin * 2));
    GPIOx->PUPDR |= (pupd << (pin * 2));
}

/**
 * GPIO 핀 출력 (High)
 */
void GPIO_SetPin(GPIO_TypeDef *GPIOx, uint8_t pin) {
    // BSRR 레지스터 사용 (원자적 연산)
    GPIOx->BSRR = (1 << pin);
}

/**
 * GPIO 핀 출력 (Low)
 */
void GPIO_ResetPin(GPIO_TypeDef *GPIOx, uint8_t pin) {
    // BSRR 상위 16비트 사용
    GPIOx->BSRR = (1 << (pin + 16));
}

/**
 * GPIO 핀 토글
 */
void GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint8_t pin) {
    GPIOx->ODR ^= (1 << pin);
}

/**
 * GPIO 핀 읽기
 */
bool GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint8_t pin) {
    return (GPIOx->IDR & (1 << pin)) ? true : false;
}

/**
 * LED 초기화 (출력 모드)
 */
void LED_Init(void) {
    // GPIOA Pin 5를 출력으로 설정 (예: STM32 Nucleo 보드 LED)
    GPIO_SetMode(GPIOA, 5, GPIO_MODE_OUTPUT);
}

/**
 * 버튼 초기화 (입력 모드, Pull-up)
 */
void Button_Init(void) {
    // GPIOB Pin 0을 입력으로 설정, Pull-up 활성화
    GPIO_SetMode(GPIOB, 0, GPIO_MODE_INPUT);
    GPIO_SetPuPd(GPIOB, 0, GPIO_PUPD_PULLUP);
}

/**
 * 사용 예제
 */
#ifdef SIMULATION
#include <stdio.h>

int main(void) {
    printf("=== GPIO 제어 예제 ===\n\n");

    // 초기화
    LED_Init();
    Button_Init();
    printf("LED 및 버튼 초기화 완료\n");

    // LED 켜기
    printf("\nLED 켜기 (PA5 = HIGH)\n");
    GPIO_SetPin(GPIOA, 5);
    printf("GPIOA ODR: 0x%08X\n", GPIOA->ODR);

    // LED 끄기
    printf("\nLED 끄기 (PA5 = LOW)\n");
    GPIO_ResetPin(GPIOA, 5);
    printf("GPIOA ODR: 0x%08X\n", GPIOA->ODR);

    // LED 토글
    printf("\nLED 토글\n");
    GPIO_TogglePin(GPIOA, 5);
    printf("GPIOA ODR: 0x%08X\n", GPIOA->ODR);

    // 버튼 상태 읽기 (시뮬레이션)
    GPIOB_SIM.IDR = 0x00000001;  // 버튼 눌림 시뮬레이션
    printf("\n버튼 상태 읽기\n");
    if (GPIO_ReadPin(GPIOB, 0)) {
        printf("버튼이 눌려있습니다.\n");
    } else {
        printf("버튼이 눌려있지 않습니다.\n");
    }

    return 0;
}
#endif
