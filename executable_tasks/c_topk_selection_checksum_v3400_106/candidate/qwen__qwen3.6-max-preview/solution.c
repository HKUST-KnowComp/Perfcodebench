#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>

static inline void sift_down(uint64_t* h, int i, int size) {
    uint64_t val = h[i];
    while (1) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int smallest = i;
        if (left < size && h[left] < h[smallest]) smallest = left;
        if (right < size && h[right] < h[smallest]) smallest = right;
        if (smallest == i) break;
        h[i] = h[smallest];
        i = smallest;
    }
    h[i] = val;
}

static int cmp_uint64_desc(const void* a, const void* b) {
    uint64_t x = *(const uint64_t*)a;
    uint64_t y = *(const uint64_t*)b;
    return (x < y) - (x > y);
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
    if (k <= 0) return 1469598103934665603ULL;
    if ((size_t)k > n) k = (int)n;

    uint64_t* heap = (uint64_t*)malloc(k * sizeof(uint64_t));
    if (!heap) return 0;

    uint64_t h = 0;
    for (int iter = 0; iter < iters; ++iter) {
        int heap_size = 0;
        for (size_t i = 0; i < n; ++i) {
            uint64_t v = ((uint64_t)scores[i] << 32) | (0xFFFFFFFFU - (uint32_t)i);
            if (heap_size < k) {
                heap[heap_size++] = v;
                if (heap_size == k) {
                    for (int j = k / 2 - 1; j >= 0; --j) sift_down(heap, j, k);
                }
            } else if (v > heap[0]) {
                heap[0] = v;
                sift_down(heap, 0, k);
            }
        }

        qsort(heap, k, sizeof(uint64_t), cmp_uint64_desc);

        h = 1469598103934665603ULL;
        for (int i = 0; i < k; ++i) {
            uint64_t v = heap[i];
            uint32_t idx = 0xFFFFFFFFU - (uint32_t)v;
            h ^= (v & 0xFFFFFFFF00000000ULL) ^ idx;
            h *= 1099511628211ULL;
        }
    }
    free(heap);
    return h;
}