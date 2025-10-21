# RTOS (Real-Time Operating System)

실시간 운영체제 프로그래밍 - 임베디드 필수 기술

## RTOS란?

실시간 운영체제는 태스크 스케줄링, 동기화, 통신 기능을 제공하여 복잡한 임베디드 시스템 개발을 단순화합니다.

## 주요 RTOS

- **FreeRTOS**: 가장 인기 있는 오픈소스 RTOS
- **Zephyr**: Linux Foundation 주관
- **ThreadX**: Microsoft (Azure RTOS)
- **RT-Thread**: 중국 오픈소스

## 학습 내용

### 1. FreeRTOS 기초 (`freertos-basics/`)
- RTOS 개념
- 태스크 생성 및 삭제
- 스케줄러

### 2. 태스크 관리 (`task-management/`)
- 우선순위
- 태스크 상태
- 컨텍스트 스위칭

### 3. 동기화 (`synchronization/`)
- Semaphore (세마포어)
- Mutex (뮤텍스)
- Queue (큐)
- Event Group

### 4. 실시간 스케줄링 (`real-time-scheduling/`)
- 우선순위 기반 스케줄링
- Rate Monotonic
- Deadline 관리

## RTOS vs Bare-Metal

| 특징 | Bare-Metal | RTOS |
|------|-----------|------|
| 복잡도 | 낮음 | 중간 |
| 메모리 사용 | 적음 | 중간 |
| 개발 속도 | 느림 | 빠름 |
| 멀티태스킹 | 수동 | 자동 |
| 적합한 프로젝트 | 간단한 제어 | 복잡한 시스템 |
