# 임베디드 면접 질문 모음

## 📋 목차

1. [C 언어](#c-언어)
2. [메모리 관리](#메모리-관리)
3. [MCU 하드웨어](#mcu-하드웨어)
4. [통신 프로토콜](#통신-프로토콜)
5. [RTOS](#rtos)
6. [디버깅](#디버깅)

---

## C 언어

### Q1: volatile 키워드는 언제 사용하나요?

**답변:**
- 레지스터 접근 시
- 인터럽트와 공유하는 변수
- 멀티스레드 환경의 공유 변수

**예제:**
```c
volatile uint32_t *GPIO_ODR = (volatile uint32_t*)0x40020014;
volatile bool flag = false;  // ISR에서 수정
```

**이유:** 컴파일러 최적화를 방지하여 항상 메모리에서 값을 읽도록 함

---

### Q2: const와 #define의 차이는?

| 특징 | const | #define |
|------|-------|---------|
| 타입 체크 | O | X |
| 디버깅 | 가능 | 어려움 |
| 메모리 | 사용 | 사용 안 함 |
| 스코프 | 있음 | 없음 |

**권장:** const 사용 (타입 안정성, 디버깅 편의성)

---

### Q3: 포인터와 배열의 차이는?

```c
char str1[] = "Hello";  // 배열: 스택에 저장, 수정 가능
char *str2 = "Hello";   // 포인터: 문자열 리터럴, 수정 불가

sizeof(str1);  // 6 (널 문자 포함)
sizeof(str2);  // 4 또는 8 (포인터 크기)
```

---

### Q4: 함수 포인터를 어디에 사용하나요?

**사용 사례:**
1. 콜백 함수
2. 상태 머신
3. 인터럽트 벡터 테이블

**예제:**
```c
typedef void (*StateHandler)(void);

StateHandler current_state = idle_state;
current_state();  // 상태 함수 실행
```

---

## 메모리 관리

### Q5: 스택과 힙의 차이는?

| 특징 | 스택 | 힙 |
|------|------|-----|
| 할당 속도 | 빠름 | 느림 |
| 크기 | 제한적 | 큼 |
| 관리 | 자동 | 수동 |
| 단편화 | 없음 | 발생 |
| 임베디드 사용 | 주로 사용 | 신중하게 |

---

### Q6: 메모리 누수를 방지하는 방법은?

**방법:**
1. malloc/free 짝 맞추기
2. 메모리 풀 사용
3. 정적 할당 선호
4. Valgrind 같은 도구 사용

**임베디드 권장:** 동적 할당 최소화, 메모리 풀 사용

---

### Q7: 구조체 패딩(Padding)이란?

```c
struct Example {
    char a;     // 1 byte
    // 3 bytes padding
    int b;      // 4 bytes
    char c;     // 1 byte
    // 3 bytes padding
};  // 총 12 bytes

// 최적화
struct Optimized {
    int b;      // 4 bytes
    char a;     // 1 byte
    char c;     // 1 byte
    // 2 bytes padding
};  // 총 8 bytes
```

**__attribute__((packed))** 사용 시 패딩 제거 (주의: 성능 저하 가능)

---

## MCU 하드웨어

### Q8: GPIO의 Pull-up/Pull-down 저항이 필요한 이유는?

**답변:**
- 플로팅(Floating) 상태 방지
- 버튼 입력: Pull-up으로 HIGH 유지, 누르면 GND로 LOW
- 노이즈 방지

---

### Q9: 인터럽트 우선순위는 어떻게 설정하나요?

**NVIC (Nested Vectored Interrupt Controller):**
```c
NVIC_SetPriority(EXTI0_IRQn, 0);  // 최고 우선순위
NVIC_SetPriority(TIM2_IRQn, 1);   // 중간
```

**원칙:**
- 긴급한 이벤트: 높은 우선순위
- 짧은 ISR 유지
- 우선순위 역전 주의

---

### Q10: PWM으로 모터 속도를 제어하는 원리는?

**원리:**
- Duty Cycle 조절로 평균 전압 변경
- 50% duty → 50% 속도
- 주파수: 보통 20kHz (인간 가청 주파수 이상)

```c
TIM2->CCR1 = 500;  // 50% duty cycle (ARR = 1000)
```

---

## 통신 프로토콜

### Q11: UART, SPI, I2C의 차이는?

| 프로토콜 | 선 개수 | 속도 | 동기/비동기 | 용도 |
|---------|--------|------|-----------|------|
| UART | 2 | 중간 | 비동기 | 디버그, 간단한 통신 |
| SPI | 4+ | 빠름 | 동기 | 고속 센서, 메모리 |
| I2C | 2 | 느림 | 동기 | 저속 센서, 주변장치 |

---

### Q12: I2C의 ACK/NACK는 무엇인가요?

**ACK (Acknowledge):**
- 수신 성공 신호
- SDA를 LOW로 당김

**NACK (Not Acknowledge):**
- 수신 실패 또는 마지막 바이트
- SDA를 HIGH로 유지

---

### Q13: SPI의 CPOL과 CPHA란?

| 모드 | CPOL | CPHA | 설명 |
|------|------|------|------|
| 0 | 0 | 0 | CLK idle LOW, 첫 엣지에서 샘플링 |
| 1 | 0 | 1 | CLK idle LOW, 두 번째 엣지에서 샘플링 |
| 2 | 1 | 0 | CLK idle HIGH, 첫 엣지에서 샘플링 |
| 3 | 1 | 1 | CLK idle HIGH, 두 번째 엣지에서 샘플링 |

**중요:** 마스터와 슬레이브가 동일한 모드 사용

---

## RTOS

### Q14: RTOS가 필요한 이유는?

**장점:**
- 멀티태스킹 쉬움
- 우선순위 기반 스케줄링
- 동기화 메커니즘 제공
- 코드 구조화

**단점:**
- 메모리 오버헤드
- 복잡도 증가

**사용 기준:** 3개 이상의 병렬 작업 필요 시

---

### Q15: Semaphore와 Mutex의 차이는?

| 특징 | Semaphore | Mutex |
|------|-----------|-------|
| 카운터 | 0 이상 | 0 또는 1 |
| 소유권 | 없음 | 있음 |
| 우선순위 상속 | X | O |
| 용도 | 신호, 카운팅 | 상호 배제 |

**Mutex:** 공유 리소스 보호
**Semaphore:** 이벤트 신호, 리소스 카운팅

---

### Q16: 우선순위 역전(Priority Inversion)이란?

**문제:**
- 낮은 우선순위 태스크가 Mutex 획득
- 높은 우선순위 태스크가 대기
- 중간 우선순위 태스크가 실행

**해결:** Mutex의 우선순위 상속 (Priority Inheritance)

---

## 디버깅

### Q17: 하드폴트(HardFault)의 원인은?

**주요 원인:**
1. 잘못된 포인터 접근 (NULL, 언정렬)
2. 스택 오버플로우
3. 잘못된 함수 포인터
4. 접근 권한 위반

**디버깅:**
```c
void HardFault_Handler(void) {
    // CFSR, HFSR, MMFAR 레지스터 확인
    volatile uint32_t *cfsr = (uint32_t*)0xE000ED28;
    printf("CFSR: 0x%08X\n", *cfsr);
    while(1);
}
```

---

### Q18: Watchdog Timer의 역할은?

**목적:** 시스템 정지 방지

**동작:**
- 주기적으로 Watchdog 리셋 필요
- 리셋 안 하면 시스템 재시작

```c
IWDG_ReloadCounter();  // 주기적으로 호출
```

---

### Q19: 바운싱(Bouncing)이란?

**문제:** 버튼 누를 때 기계적 떨림으로 여러 번 입력 감지

**해결:**
1. 하드웨어: RC 필터
2. 소프트웨어: 디바운싱 지연

```c
#define DEBOUNCE_TIME 50  // ms

if (current_time - last_time > DEBOUNCE_TIME) {
    // 버튼 입력 처리
}
```

---

### Q20: 임베디드에서 printf 사용 시 주의점은?

**문제:**
- 느림 (UART 전송)
- 스택 사용량 많음
- ISR에서 사용 금지

**대안:**
- 릴리스 빌드에서 제거
- 경량 로깅 라이브러리 사용
- ITM (Instrumentation Trace Macrocell) 사용

---

## 🎯 면접 팁

1. **이론보다 실무 경험 강조**
   - "프로젝트에서 이렇게 사용했습니다"

2. **트레이드오프 설명**
   - "이 방식의 장단점은..."

3. **코드로 설명**
   - 화이트보드 코딩 준비

4. **모르면 솔직히**
   - "잘 모르지만, 이렇게 접근하겠습니다"

5. **질문 역으로 하기**
   - "귀사에서는 어떤 RTOS를 사용하시나요?"
