#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static inline int is_worse(Pair a, Pair b) {
    if (a.score != b.score) return a.score < b.score;
    return a.index > b.index;
}

static inline void sift_down(Pair* heap, int k, int i) {
    while (1) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int worst = i;
        if (left < k && is_worse(heap[left], heap[worst])) worst = left;
        if (right < k && is_worse(heap[right], heap[worst])) worst = right;
        if (worst == i) break;
        Pair tmp = heap[i];
        heap[i] = heap[worst];
        heap[worst] = tmp;
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
  if (k <= 0) return 0;
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
      sift_down(heap, k, i);
    }
    
    for (size_t i = k; i < n; ++i) {
      uint32_t s = scores[i];
      uint32_t idx = (uint32_t)i;
      if (s > heap[0].score || (s == heap[0].score && idx < heap[0].index)) {
        heap[0].score = s;
        heap[0].index = idx;
        sift_down(heap, k, 0);
      }
    }
    
    qsort(heap, k, sizeof(Pair), cmp_pair_desc);
    
    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      h ^= ((uint64_t)heap[i].score << 32) ^ heap[i].index;
      h *= 1099511628211ULL;
    }
  }
  
  free(heap);
  return h;
}
