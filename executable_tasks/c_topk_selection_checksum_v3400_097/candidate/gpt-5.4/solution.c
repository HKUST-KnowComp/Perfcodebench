#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static inline int pair_better(Pair a, Pair b) {
  if (a.score != b.score) return a.score > b.score;
  return a.index < b.index;
}

static inline int pair_worse(Pair a, Pair b) {
  if (a.score != b.score) return a.score < b.score;
  return a.index > b.index;
}

static inline void swap_pair(Pair* a, Pair* b) {
  Pair t = *a;
  *a = *b;
  *b = t;
}

static void sift_down(Pair* heap, int start, int count) {
  int root = start;
  while (1) {
    int child = root * 2 + 1;
    if (child >= count) break;
    int right = child + 1;
    int smallest = child;
    if (right < count && pair_worse(heap[right], heap[child])) smallest = right;
    if (!pair_worse(heap[smallest], heap[root])) break;
    swap_pair(&heap[root], &heap[smallest]);
    root = smallest;
  }
}

static void heap_sort_desc(Pair* a, int n) {
  for (int start = (n - 2) / 2; start >= 0; --start) sift_down(a, start, n);
  for (int end = n - 1; end > 0; --end) {
    swap_pair(&a[0], &a[end]);
    sift_down(a, 0, end);
  }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  if (k <= 0 || n == 0 || iters <= 0) return 1469598103934665603ULL;
  if ((size_t)k > n) k = (int)n;

  Pair* top = (Pair*)malloc((size_t)k * sizeof(Pair));
  if (!top) return 0;

  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (int i = 0; i < k; ++i) {
      top[i].score = scores[i];
      top[i].index = (uint32_t)i;
    }

    for (int start = (k - 2) / 2; start >= 0; --start) sift_down(top, start, k);

    for (size_t i = (size_t)k; i < n; ++i) {
      Pair p;
      p.score = scores[i];
      p.index = (uint32_t)i;
      if (pair_better(p, top[0])) {
        top[0] = p;
        sift_down(top, 0, k);
      }
    }

    heap_sort_desc(top, k);

    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      h ^= ((uint64_t)top[i].score << 32) ^ top[i].index;
      h *= 1099511628211ULL;
    }
  }

  free(top);
  return h;
}
