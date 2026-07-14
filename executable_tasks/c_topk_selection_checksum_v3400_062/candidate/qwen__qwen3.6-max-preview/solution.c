#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static inline int is_greater(Pair a, Pair b) {
    if (a.score != b.score) return a.score > b.score;
    return a.index < b.index;
}

static inline void sift_down(Pair* heap, int start, int end) {
    int root = start;
    while (1) {
        int child = 2 * root + 1;
        if (child > end) break;
        if (child + 1 <= end && is_greater(heap[child], heap[child+1])) {
            child++;
        }
        if (is_greater(heap[root], heap[child])) {
            Pair tmp = heap[root];
            heap[root] = heap[child];
            heap[child] = tmp;
            root = child;
        } else {
            break;
        }
    }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
    if (k <= 0 || n == 0) return 0;
    if ((size_t)k > n) k = (int)n;
    
    Pair* heap = (Pair*)malloc(k * sizeof(Pair));
    if (!heap) return 0;
    
    uint64_t h = 0;
    for (int iter = 0; iter < iters; ++iter) {
        for (int i = 0; i < k; ++i) {
            heap[i].score = scores[i];
            heap[i].index = (uint32_t)i;
        }
        
        for (int i = k / 2 - 1; i >= 0; --i) {
            sift_down(heap, i, k - 1);
        }
        
        for (size_t i = (size_t)k; i < n; ++i) {
            uint32_t s = scores[i];
            if (s > heap[0].score || (s == heap[0].score && (uint32_t)i < heap[0].index)) {
                heap[0].score = s;
                heap[0].index = (uint32_t)i;
                sift_down(heap, 0, k - 1);
            }
        }
        
        for (int i = k - 1; i > 0; --i) {
            Pair tmp = heap[0];
            heap[0] = heap[i];
            heap[i] = tmp;
            sift_down(heap, 0, i - 1);
        }
        
        h = 1469598103934665603ULL;
        for (int i = 0; i < k; ++i) {
            h ^= ((uint64_t)heap[i].score << 32) ^ heap[i].index;
            h *= 1099511628211ULL;
        }
    }
    free(heap);
    return h;
}