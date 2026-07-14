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

static inline int is_smaller(const Pair* a, const Pair* b) {
  if (a->score != b->score) return a->score < b->score;
  return a->index > b->index;
}

static inline void sift_down(Pair* heap, int size, int i) {
  while (1) {
    int smallest = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;
    if (l < size && is_smaller(&heap[l], &heap[smallest])) smallest = l;
    if (r < size && is_smaller(&heap[r], &heap[smallest])) smallest = r;
    if (smallest == i) break;
    Pair tmp = heap[i];
    heap[i] = heap[smallest];
    heap[smallest] = tmp;
    i = smallest;
  }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  if (k <= 0) return 1469598103934665603ULL;
  if ((size_t)k > n) k = (int)n;

  Pair* pairs = (Pair*)malloc(n * sizeof(Pair));
  if (!pairs) return 0;

  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (size_t i = 0; i < n; ++i) {
      pairs[i].score = scores[i];
      pairs[i].index = (uint32_t)i;
    }

    for (int i = k / 2 - 1; i >= 0; --i) {
      sift_down(pairs, k, i);
    }

    for (size_t i = k; i < n; ++i) {
      if (is_smaller(&pairs[0], &pairs[i])) {
        pairs[0] = pairs[i];
        sift_down(pairs, k, 0);
      }
    }

    qsort(pairs, k, sizeof(Pair), cmp_pair_desc);

    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
      h *= 1099511628211ULL;
    }
  }
  free(pairs);
  return h;
}