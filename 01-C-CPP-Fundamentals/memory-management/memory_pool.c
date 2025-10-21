/**
 * 메모리 풀 (Memory Pool)
 *
 * 임베디드에서 동적 메모리 할당 대안
 * - malloc/free는 메모리 단편화 유발
 * - 메모리 풀은 고정 크기 블록을 미리 할당
 * - 실시간 시스템에서 결정적인 동작 보장
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define POOL_SIZE 10
#define BLOCK_SIZE 64

typedef struct {
    uint8_t data[BLOCK_SIZE];
    bool is_used;
} MemoryBlock;

typedef struct {
    MemoryBlock blocks[POOL_SIZE];
    uint32_t used_count;
} MemoryPool;

// 전역 메모리 풀
static MemoryPool mem_pool;

/**
 * 메모리 풀 초기화
 */
void mem_pool_init(void) {
    for (int i = 0; i < POOL_SIZE; i++) {
        mem_pool.blocks[i].is_used = false;
        memset(mem_pool.blocks[i].data, 0, BLOCK_SIZE);
    }
    mem_pool.used_count = 0;

    printf("메모리 풀 초기화 완료 (%d 블록, 블록 크기: %d 바이트)\n",
           POOL_SIZE, BLOCK_SIZE);
}

/**
 * 메모리 블록 할당
 */
void* mem_pool_alloc(void) {
    for (int i = 0; i < POOL_SIZE; i++) {
        if (!mem_pool.blocks[i].is_used) {
            mem_pool.blocks[i].is_used = true;
            mem_pool.used_count++;

            printf("블록 할당: #%d (사용 중: %d/%d)\n",
                   i, mem_pool.used_count, POOL_SIZE);

            return mem_pool.blocks[i].data;
        }
    }

    printf("메모리 풀 가득 참! 할당 실패\n");
    return NULL;  // 메모리 풀 고갈
}

/**
 * 메모리 블록 해제
 */
bool mem_pool_free(void *ptr) {
    if (ptr == NULL) {
        return false;
    }

    for (int i = 0; i < POOL_SIZE; i++) {
        if (mem_pool.blocks[i].data == ptr) {
            if (!mem_pool.blocks[i].is_used) {
                printf("경고: 이미 해제된 블록 #%d\n", i);
                return false;
            }

            mem_pool.blocks[i].is_used = false;
            memset(mem_pool.blocks[i].data, 0, BLOCK_SIZE);
            mem_pool.used_count--;

            printf("블록 해제: #%d (사용 중: %d/%d)\n",
                   i, mem_pool.used_count, POOL_SIZE);

            return true;
        }
    }

    printf("오류: 잘못된 포인터 주소\n");
    return false;
}

/**
 * 메모리 풀 상태 출력
 */
void mem_pool_status(void) {
    printf("\n메모리 풀 상태:\n");
    printf("사용 중: %d / %d 블록\n", mem_pool.used_count, POOL_SIZE);
    printf("블록 상태: [");

    for (int i = 0; i < POOL_SIZE; i++) {
        printf("%c", mem_pool.blocks[i].is_used ? 'X' : 'O');
        if (i < POOL_SIZE - 1) printf(" ");
    }
    printf("]\n");
    printf("           (O: 사용 가능, X: 사용 중)\n\n");
}

/**
 * 사용 예제
 */
int main(void) {
    printf("=== 메모리 풀 (Memory Pool) ===\n\n");

    mem_pool_init();
    mem_pool_status();

    // 메모리 할당
    printf("--- 메모리 블록 3개 할당 ---\n");
    uint8_t *block1 = (uint8_t*)mem_pool_alloc();
    uint8_t *block2 = (uint8_t*)mem_pool_alloc();
    uint8_t *block3 = (uint8_t*)mem_pool_alloc();
    mem_pool_status();

    // 메모리 사용
    if (block1 != NULL) {
        sprintf((char*)block1, "센서 데이터 1");
        printf("block1에 데이터 저장: %s\n", block1);
    }

    // 메모리 해제
    printf("\n--- 블록 2 해제 ---\n");
    mem_pool_free(block2);
    mem_pool_status();

    // 재할당
    printf("--- 블록 재할당 ---\n");
    uint8_t *block4 = (uint8_t*)mem_pool_alloc();
    mem_pool_status();

    // 모두 해제
    printf("--- 모든 블록 해제 ---\n");
    mem_pool_free(block1);
    mem_pool_free(block3);
    mem_pool_free(block4);
    mem_pool_status();

    return 0;
}
