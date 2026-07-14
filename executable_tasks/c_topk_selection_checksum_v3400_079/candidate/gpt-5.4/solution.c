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

static void sift_down(Pair* heap, int start, int count) {
  int root = start;
  for (;;) {
    int child = root * 2 + 1;
    if (child >= count) break;
    int right = child + 1;
    int smallest = child;
    if (right < count && pair_better(heap[child], heap[right])) {
      smallest = right;
    }
    if (!pair_better(heap[root], heap[smallest])) break;
    swap_pair(&heap[root], &heap[smallest]);
    root = smallest;
  }
}

static void heapify(Pair* heap, int count) {
  for (int i = (count - 2) / 2; i >= 0; --i) {
    sift_down(heap, i, count);
  }
}

static void sort_topk_desc(Pair* a, int k) {
  heapify(a, k);
  for (int end = k - 1; end > 0; --end) {
    swap_pair(&a[0], &a[end]);
    sift_down(a, 0, end);
  }
  for (int i = 0, j = k - 1; i < j; ++i, --j) {
    swap_pair(&a[i], &a[j]);
  }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  if (k <= 0 || n == 0 || iters <= 0) {
    return 1469598103934665603ULL;
  }

  if ((size_t)k > n) k = (int)n;

  Pair* top = (Pair*)malloc((size_t)k * sizeof(Pair));
  if (!top) return 0;

  uint64_t h = 1469598103934665603ULL;

  for (int iter = 0; iter < iters; ++iter) {
    int m = 0;
    for (; m < k; ++m) {
      top[m].score = scores[m];
      top[m].index = (uint32_t)m;
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
