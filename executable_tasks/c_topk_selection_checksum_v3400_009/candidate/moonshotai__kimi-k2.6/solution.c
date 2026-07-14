#include <stdint.h>
#include <stdlib.h>

#ifdef __GNUC__
#define UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define UNLIKELY(x) (x)
#endif

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static inline int pair_is_worse_val(uint32_t a_score, uint32_t a_index,
                                    uint32_t b_score, uint32_t b_index) {
  if (a_score < b_score) return 1;
  if (a_score > b_score) return 0;
  return a_index > b_index;
}

static inline void heap_sift_down(Pair* heap, size_t root, size_t n) {
  uint32_t tmp_score = heap[root].score;
  uint32_t tmp_index = heap[root].index;
  size_t child;
  while ((child = root * 2 + 1) < n) {
    size_t right = child + 1;
    if (right < n && pair_is_worse_val(heap[right].score, heap[right].index,
                                       heap[child].score, heap[child].index)) {
      child = right;
    }
    if (pair_is_worse_val(tmp_score, tmp_index,
                          heap[child].score, heap[child].index)) {
      break;
    }
    heap[root].score = heap[child].score;
    heap[root].index = heap[child].index;
    root = child;
  }
  heap[root].score = tmp_score;
  heap[root].index = tmp_index;
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  if (iters <= 0) return 0;
  if (k <= 0 || n == 0) return 1469598103934665603ULL;

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

    for (size_t i = kk / 2; i-- > 0; ) {
      heap_sift_down(heap, i, kk);
    }

    for (size_t i = kk; i < n; ++i) {
      uint32_t s = scores[i];
      uint32_t idx = (uint32_t)i;
      if (UNLIKELY(s > heap[0].score ||
                   (s == heap[0].score && idx < heap[0].index))) {
        heap[0].score = s;
        heap[0].index = idx;
        heap_sift_down(heap, 0, kk);
      }
    }

    for (size_t i = kk; i-- > 1; ) {
      Pair tmp = heap[0];
      heap[0] = heap[i];
      heap[i] = tmp;
      heap_sift_down(heap, 0, i);
    }

    h = 1469598103934665603ULL;
    for (size_t i = 0; i < kk; ++i) {
      h ^= ((uint64_t)heap[i].score << 32) ^ heap[i].index;
      h *= 1099511628211ULL;
    }
  }

  free(heap);
  return h;
}
