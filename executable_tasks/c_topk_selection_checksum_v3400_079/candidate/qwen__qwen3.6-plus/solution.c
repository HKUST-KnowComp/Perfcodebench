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

static inline int pair_gt(Pair a, Pair b) {
    if (a.score != b.score) return a.score > b.score;
    return a.index < b.index;
}

static inline int pair_lt(Pair a, Pair b) {
    if (a.score != b.score) return a.score < b.score;
    return a.index > b.index;
}

static inline void sift_down(Pair* heap, int k, int i) {
    Pair val = heap[i];
    while (1) {
        int left = 2 * i + 1;
        if (left >= k) break;
        int right = left + 1;
        int smallest = left;
        if (right < k && pair_lt(heap[right], heap[left])) {
            smallest = right;
        }
        if (pair_lt(val, heap[smallest])) break;
        heap[i] = heap[smallest];
        i = smallest;
    }
    heap[i] = val;
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  if (k <= 0) return 1469598103934665603ULL;
  if (k > (int)n) k = (int)n;
  
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
      Pair cur = {scores[i], (uint32_t)i};
      if (pair_gt(cur, heap[0])) {
        heap[0] = cur;
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