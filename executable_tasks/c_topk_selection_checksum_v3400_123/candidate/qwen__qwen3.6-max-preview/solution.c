#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>

static inline void sift_down(uint64_t* h, int k, int i) {
    uint64_t val = h[i];
    int child;
    while ((child = 2 * i + 1) < k) {
        if (child + 1 < k && h[child + 1] < h[child]) child++;
        if (val <= h[child]) break;
        h[i] = h[child];
        i = child;
    }
    h[i] = val;
}

static int cmp_u64_desc(const void* a, const void* b) {
    uint64_t x = *(const uint64_t*)a;
    uint64_t y = *(const uint64_t*)b;
    return (x > y) ? -1 : (x < y) ? 1 : 0;
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
    if (k <= 0) return 0;
    if ((size_t)k > n) k = (int)n;

    uint64_t* heap = (uint64_t*)malloc(k * sizeof(uint64_t));
    if (!heap) return 0;

    uint64_t h = 0;
    for (int iter = 0; iter < iters; ++iter) {
        for (int i = 0; i < k; ++i) {
            heap[i] = ((uint64_t)scores[i] << 32) | (0xFFFFFFFFU - (uint32_t)i);
        }
        for (int i = k / 2 - 1; i >= 0; --i) {
            sift_down(heap, k, i);
        }
        for (size_t i = k; i < n; ++i) {
            uint64_t val = ((uint64_t)scores[i] << 32) | (0xFFFFFFFFU - (uint32_t)i);
            if (val > heap[0]) {
                heap[0] = val;
                sift_down(heap, k, 0);
            }
        }
        qsort(heap, k, sizeof(uint64_t), cmp_u64_desc);

        h = 1469598103934665603ULL;
        for (int i = 0; i < k; ++i) {
            uint32_t score = (uint32_t)(heap[i] >> 32);
            uint32_t index = 0xFFFFFFFFU - (uint32_t)heap[i];
            h ^= ((uint64_t)score << 32) ^ index;
            h *= 1099511628211ULL;
        }
    }
    free(heap);
    return h;
}