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
  for (;;) {
    int child = root * 2 + 1;
    if (child >= size) break;
    int right = child + 1;
    int smallest = root;

    if (pair_better(heap[smallest], heap[child])) smallest = child;
    if (right < size && pair_better(heap[smallest], heap[right])) smallest = right;
    if (smallest == root) break;
    swap_pair(&heap[root], &heap[smallest]);
    root = smallest;
  }
}

static void sort_topk_desc(Pair* a, int k) {
  for (int i = k / 2 - 1; i >= 0; --i) sift_down(a, i, k);
  for (int end = k - 1; end > 0; --end) {
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
    int heap_size = 0;

    for (size_t i = 0; i < (size_t)k; ++i) {
      top[i].score = scores[i];
      top[i].index = (uint32_t)i;
      ++heap_size;
    }

    for (int i = heap_size / 2 - 1; i >= 0; --i) sift_down(top, i, heap_size);

    for (size_t i = (size_t)k; i < n; ++i) {
      Pair cand;
      cand.score = scores[i];
      cand.index = (uint32_t)i;
      if (pair_better(cand, top[0])) {
        top[0] = cand;
        sift_down(top, 0, heap_size);
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
