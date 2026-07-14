#include <stdint.h>
#include <stdlib.h>

static inline void sift_down(uint64_t* heap, int start, int end) {
    int root = start;
    while (1) {
        int child = 2 * root + 1;
        if (child >= end) break;
        int swap = root;
        if (heap[child] < heap[swap]) swap = child;
        if (child + 1 < end && heap[child + 1] < heap[swap]) swap = child + 1;
        if (swap == root) break;
        uint64_t tmp = heap[root]; heap[root] = heap[swap]; heap[swap] = tmp;
        root = swap;
    }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
    if (k <= 0) return 1469598103934665603ULL;
    if ((size_t)k > n) k = (int)n;

    uint64_t* heap = (uint64_t*)malloc(k * sizeof(uint64_t));
    if (!heap) return 0;

    uint64_t h = 0;
    for (int iter = 0; iter < iters; ++iter) {
        for (int i = 0; i < k; ++i) {
            heap[i] = ((uint64_t)scores[i] << 32) | (0xFFFFFFFFU ^ (uint32_t)i);
        }

        for (int i = k / 2 - 1; i >= 0; --i) {
            sift_down(heap, i, k);
        }

        for (size_t i = k; i < n; ++i) {
            uint64_t val = ((uint64_t)scores[i] << 32) | (0xFFFFFFFFU ^ (uint32_t)i);
            if (val > heap[0]) {
                heap[0] = val;
                sift_down(heap, 0, k);
            }
        }

        for (int end = k - 1; end > 0; --end) {
            uint64_t tmp = heap[0]; heap[0] = heap[end]; heap[end] = tmp;
            sift_down(heap, 0, end);
        }

        h = 1469598103934665603ULL;
        for (int i = k - 1; i >= 0; --i) {
            uint64_t val = heap[i];
            uint32_t score = (uint32_t)(val >> 32);
            uint32_t index = 0xFFFFFFFFU ^ (uint32_t)val;
            h ^= ((uint64_t)score << 32) ^ index;
            h *= 1099511628211ULL;
        }
    }

    free(heap);
    return h;
}