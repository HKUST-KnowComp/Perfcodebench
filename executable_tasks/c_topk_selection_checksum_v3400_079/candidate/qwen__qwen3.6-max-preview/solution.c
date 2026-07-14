#include <stdint.h>
#include <stdlib.h>

static inline void sift_down(uint64_t* h, int i, int size) {
    uint64_t val = h[i];
    int child;
    while ((child = 2 * i + 1) < size) {
        if (child + 1 < size && h[child + 1] < h[child]) child++;
        if (val <= h[child]) break;
        h[i] = h[child];
        i = child;
    }
    h[i] = val;
}

static int cmp_uint64_desc(const void* a, const void* b) {
    uint64_t va = *(const uint64_t*)a;
    uint64_t vb = *(const uint64_t*)b;
    return (va > vb) ? -1 : (va < vb) ? 1 : 0;
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
    if (k <= 0 || n == 0) return 0;
    if ((size_t)k > n) k = (int)n;

    uint64_t* packed = (uint64_t*)malloc(n * sizeof(uint64_t));
    uint64_t* heap = (uint64_t*)malloc(k * sizeof(uint64_t));
    if (!packed || !heap) { free(packed); free(heap); return 0; }

    for (size_t i = 0; i < n; ++i) {
        packed[i] = ((uint64_t)scores[i] << 32) | ~(uint32_t)i;
    }

    uint64_t h = 0;
    for (int iter = 0; iter < iters; ++iter) {
        for (int i = 0; i < k; ++i) heap[i] = packed[i];
        for (int i = k / 2 - 1; i >= 0; --i) sift_down(heap, i, k);

        for (size_t i = k; i < n; ++i) {
            uint64_t val = packed[i];
            if (val > heap[0]) {
                heap[0] = val;
                sift_down(heap, 0, k);
            }
        }

        qsort(heap, k, sizeof(uint64_t), cmp_uint64_desc);

        h = 1469598103934665603ULL;
        for (int i = 0; i < k; ++i) {
            uint32_t score = (uint32_t)(heap[i] >> 32);
            uint32_t index = ~(uint32_t)heap[i];
            h ^= ((uint64_t)score << 32) ^ index;
            h *= 1099511628211ULL;
        }
    }

    free(packed);
    free(heap);
    return h;
}