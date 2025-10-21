/**
 * 환형 버퍼 (Circular Buffer / Ring Buffer)
 *
 * 임베디드에서 가장 많이 사용되는 자료구조
 * - UART 수신 버퍼
 * - 센서 데이터 버퍼
 * - 로깅 버퍼
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define BUFFER_SIZE 8

typedef struct {
    uint8_t buffer[BUFFER_SIZE];
    uint8_t head;    // 쓰기 위치
    uint8_t tail;    // 읽기 위치
    uint8_t count;   // 현재 데이터 개수
} CircularBuffer;

/**
 * 버퍼 초기화
 */
void cbuf_init(CircularBuffer *cb) {
    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;
    memset(cb->buffer, 0, BUFFER_SIZE);
}

/**
 * 버퍼가 비어있는지 확인
 */
bool cbuf_is_empty(CircularBuffer *cb) {
    return (cb->count == 0);
}

/**
 * 버퍼가 가득 찼는지 확인
 */
bool cbuf_is_full(CircularBuffer *cb) {
    return (cb->count == BUFFER_SIZE);
}

/**
 * 데이터 쓰기
 */
bool cbuf_write(CircularBuffer *cb, uint8_t data) {
    if (cbuf_is_full(cb)) {
        return false;  // 버퍼 가득 찼음
    }

    cb->buffer[cb->head] = data;
    cb->head = (cb->head + 1) % BUFFER_SIZE;  // 환형으로 이동
    cb->count++;

    return true;
}

/**
 * 데이터 읽기
 */
bool cbuf_read(CircularBuffer *cb, uint8_t *data) {
    if (cbuf_is_empty(cb)) {
        return false;  // 버퍼 비어있음
    }

    *data = cb->buffer[cb->tail];
    cb->tail = (cb->tail + 1) % BUFFER_SIZE;  // 환형으로 이동
    cb->count--;

    return true;
}

/**
 * 버퍼 상태 출력
 */
void cbuf_print_status(CircularBuffer *cb) {
    printf("버퍼 상태: [");
    for (int i = 0; i < BUFFER_SIZE; i++) {
        if (i == cb->tail && i == cb->head && cb->count == 0) {
            printf(" T/H ");
        } else if (i == cb->tail) {
            printf(" T ");
        } else if (i == cb->head) {
            printf(" H ");
        } else {
            printf("   ");
        }
    }
    printf("]\n");

    printf("데이터:   [");
    for (int i = 0; i < BUFFER_SIZE; i++) {
        printf(" %02d ", cb->buffer[i]);
    }
    printf("] (개수: %d)\n", cb->count);
}

/**
 * 사용 예제
 */
int main(void) {
    CircularBuffer cb;
    uint8_t data;

    printf("=== 환형 버퍼 (Circular Buffer) ===\n\n");

    cbuf_init(&cb);
    printf("초기화 후:\n");
    cbuf_print_status(&cb);
    printf("\n");

    // 데이터 쓰기
    printf("데이터 쓰기: 10, 20, 30, 40, 50\n");
    cbuf_write(&cb, 10);
    cbuf_write(&cb, 20);
    cbuf_write(&cb, 30);
    cbuf_write(&cb, 40);
    cbuf_write(&cb, 50);
    cbuf_print_status(&cb);
    printf("\n");

    // 데이터 읽기
    printf("데이터 2개 읽기:\n");
    cbuf_read(&cb, &data);
    printf("읽은 데이터: %d\n", data);
    cbuf_read(&cb, &data);
    printf("읽은 데이터: %d\n", data);
    cbuf_print_status(&cb);
    printf("\n");

    // 추가 쓰기 (환형 동작 확인)
    printf("데이터 추가 쓰기: 60, 70, 80\n");
    cbuf_write(&cb, 60);
    cbuf_write(&cb, 70);
    cbuf_write(&cb, 80);
    cbuf_print_status(&cb);
    printf("\n");

    // 버퍼 가득 채우기
    printf("버퍼 가득 채우기: 90, 100\n");
    if (cbuf_write(&cb, 90)) {
        printf("90 쓰기 성공\n");
    }
    if (cbuf_write(&cb, 100)) {
        printf("100 쓰기 성공\n");
    }
    if (!cbuf_write(&cb, 110)) {
        printf("110 쓰기 실패 - 버퍼 가득 참!\n");
    }
    cbuf_print_status(&cb);

    return 0;
}
