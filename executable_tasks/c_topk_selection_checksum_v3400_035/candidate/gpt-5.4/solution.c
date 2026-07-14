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

static inline void swap_pair(Pair* a, Pair* b) {
  Pair t = *a;
  *a = *b;
  *b = t;
}

static void sift_down(Pair* heap, int start, int size) {
  int root = start;
  while (1) {
    int child = root * 2 + 1;
    if (child >= size) break;
    int right = child + 1;
    int smallest = child;
    if (right < size && pair_better(heap[child], heap[right])) {
      smallest = right;
    }
    if (!pair_better(heap[smallest], heap[root])) break;
    swap_pair(&heap[root], &heap[smallest]);
    root = smallest;
  }
}

static void heapify(Pair* heap, int size) {
  for (int i = (size >> 1) - 1; i >= 0; --i) {
    sift_down(heap, i, size);
  }
}

static void sort_topk_desc(Pair* a, int k) {
  for (int i = 1; i < k; ++i) {
    Pair v = a[i];
    int j = i;
    while (j > 0 && pair_better(v, a[j - 1])) {
      a[j] = a[j - 1];
      --j;
    }
    a[j] = v;
  }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  if (iters <= 0) return 0;
  if (k <= 0 || n == 0) {
    uint64_t h = 0;
    for (int iter = 0; iter < iters; ++iter) {
      h = 1469598103934665603ULL;
    }
    return h;
  }

  if ((size_t)k > n) k = (int)n;

  Pair* top = (Pair*)malloc((size_t)k * sizeof(Pair));
  if (!top) return 0;

  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (int i = 0; i < k; ++i) {
      top[i].score = scores[i];
      top[i].index = (uint32_t)i;
    }
    heapify(top, k);

    for (size_t i = (size_t)k; i < n; ++i) {
      Pair cand;
      cand.score = scores[i];
      cand.index = (uint32_t)i;
      if (pair_better(cand, top[0])) {
        top[0] = cand;
        sift_down(top, 0, k);
      }
    }

    sort_topk_desc(top, k);

    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      h ^= ((uint64_t)top[i].score << 32) ^ top[i].index;
      h *= 1099511628211ULL;
    }
  }

  free(top);
  return h;
}
