#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static inline int is_worse(Pair a, Pair b) {
    if (a.score < b.score) return 1;
    if (a.score > b.score) return 0;
    return a.index > b.index;
}

static void sift_down(Pair* heap, size_t size, size_t i) {
    while (1) {
        size_t left = 2 * i + 1;
        size_t right = 2 * i + 2;
        size_t worst = i;
        if (left < size && is_worse(heap[left], heap[worst])) worst = left;
        if (right < size && is_worse(heap[right], heap[worst])) worst = right;
        if (worst == i) break;
        Pair tmp = heap[i]; heap[i] = heap[worst]; heap[worst] = tmp;
        i = worst;
    }
}

static int cmp_pair_desc(const void* lhs, const void* rhs) {
  const Pair* a = (const Pair*) lhs;
  const Pair* b = (const Pair*) rhs;
  if (a->score < b->score) return 1;
  if (a->score > b->score) return -1;
  if (a->index < b->index) return -1;
  if (a->index > b->index) return 1;
  return 0;
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
    if (n == 0 || k <= 0) return 0;
    size_t kk = (size_t)k;
    if (kk > n) kk = n;
    
    Pair* heap = (Pair*)malloc(kk * sizeof(Pair));
    if (!heap) return 0;
    
    uint64_t h = 0;
    for (int iter = 0; iter < iters; ++iter) {
        for (size_t i = 0; i < kk; ++i) {
            heap[i].score = scores[i];
            heap[i].index = (uint32_t)i;
        }
        for (size_t i = kk / 2; i > 0; ) {
            sift_down(heap, kk, --i);
        }
        
        for (size_t i = kk; i < n; ++i) {
            Pair cur;
            cur.score = scores[i];
            cur.index = (uint32_t)i;
            if (!is_worse(cur, heap[0])) {
                heap[0] = cur;
                sift_down(heap, kk, 0);
            }
        }
        
        qsort(heap, kk, sizeof(Pair), cmp_pair_desc);
        
        h = 1469598103934665603ULL;
        for (size_t i = 0; i < kk; ++i) {
            h ^= ((uint64_t)heap[i].score << 32) ^ heap[i].index;
            h *= 1099511628211ULL;
        }
    }
    free(heap);
    return h;
}
