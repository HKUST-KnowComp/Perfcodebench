#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static int cmp_pair_desc(const void* lhs, const void* rhs) {
  const Pair* a = (const Pair*) lhs;
  const Pair* b = (const Pair*) rhs;
  if (a->score < b->score) return 1;
  if (a->score > b->score) return -1;
  if (a->index < b->index) return -1;
  if (a->index > b->index) return 1;
  return 0;
}

static inline int is_greater(const Pair* a, const Pair* b) {
    if (a->score != b->score) return a->score > b->score;
    return a->index < b->index;
}

static void sift_down(Pair* heap, size_t size, size_t idx) {
    Pair val = heap[idx];
    while (1) {
        size_t child = 2 * idx + 1;
        if (child >= size) break;
        if (child + 1 < size && is_greater(&heap[child], &heap[child + 1])) {
            child++;
        }
        if (!is_greater(&val, &heap[child])) break;
        heap[idx] = heap[child];
        idx = child;
    }
    heap[idx] = val;
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
    if (k <= 0) return 1469598103934665603ULL;
    if ((size_t)k > n) k = (int)n;
    size_t k_sz = (size_t)k;
    
    Pair* pairs = (Pair*)malloc(k_sz * sizeof(Pair));
    if (!pairs) return 0;
    
    uint64_t h = 0;
    for (int iter = 0; iter < iters; ++iter) {
        for (size_t i = 0; i < k_sz; ++i) {
            pairs[i].score = scores[i];
            pairs[i].index = (uint32_t)i;
        }
        
        for (size_t i = k_sz / 2; i-- > 0; ) {
            sift_down(pairs, k_sz, i);
        }
        
        for (size_t i = k_sz; i < n; ++i) {
            Pair p = {scores[i], (uint32_t)i};
            if (is_greater(&p, &pairs[0])) {
                pairs[0] = p;
                sift_down(pairs, k_sz, 0);
            }
        }
        
        qsort(pairs, k_sz, sizeof(Pair), cmp_pair_desc);
        
        h = 1469598103934665603ULL;
        for (size_t i = 0; i < k_sz; ++i) {
            h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
            h *= 1099511628211ULL;
        }
    }
    free(pairs);
    return h;
}
