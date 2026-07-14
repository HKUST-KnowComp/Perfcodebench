#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static inline int pair_less(Pair a, Pair b) {
    if (a.score < b.score) return 1;
    if (a.score > b.score) return 0;
    return a.index > b.index;
}

static void sift_down(Pair* heap, size_t k, size_t idx) {
    while (1) {
        size_t left = 2 * idx + 1;
        if (left >= k) break;
        size_t right = left + 1;
        size_t smallest = left;
        if (right < k && pair_less(heap[right], heap[left])) {
            smallest = right;
        }
        if (pair_less(heap[smallest], heap[idx])) {
            Pair tmp = heap[idx];
            heap[idx] = heap[smallest];
            heap[smallest] = tmp;
            idx = smallest;
        } else {
            break;
        }
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
  if (k <= 0) {
      return 1469598103934665603ULL;
  }
  size_t kk = (size_t)k;
  if (kk > n) kk = n;

  Pair* pairs = (Pair*)malloc(n * sizeof(Pair));
  if (!pairs) return 0;
  
  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (size_t i = 0; i < n; ++i) {
      pairs[i].score = scores[i];
      pairs[i].index = (uint32_t)i;
    }

    if (kk > n / 2) {
        qsort(pairs, n, sizeof(Pair), cmp_pair_desc);
    } else {
        for (size_t i = kk / 2; i-- > 0; ) {
            sift_down(pairs, kk, i);
        }
        for (size_t i = kk; i < n; ++i) {
            if (pair_less(pairs[0], pairs[i])) {
                pairs[0] = pairs[i];
                sift_down(pairs, kk, 0);
            }
        }
        qsort(pairs, kk, sizeof(Pair), cmp_pair_desc);
    }

    h = 1469598103934665603ULL;
    for (size_t i = 0; i < kk; ++i) {
      h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
      h *= 1099511628211ULL;
    }
  }
  free(pairs);
  return h;
}