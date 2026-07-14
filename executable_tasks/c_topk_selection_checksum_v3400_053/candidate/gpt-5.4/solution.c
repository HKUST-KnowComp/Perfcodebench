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
    int smallest = child;
    if (right < size && pair_better(heap[child], heap[right])) {
      smallest = right;
    }
    if (!pair_better(heap[smallest], heap[root])) break;
    swap_pair(&heap[root], &heap[smallest]);
    root = smallest;
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
  if (k <= 0 || n == 0 || iters <= 0) return 1469598103934665603ULL;
  if ((size_t)k > n) k = (int)n;

  Pair* top = (Pair*)malloc((size_t)k * sizeof(Pair));
  if (!top) return 0;

  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    int heap_size = 0;

    for (size_t i = 0; i < n; ++i) {
      Pair p;
      p.score = scores[i];
      p.index = (uint32_t)i;

      if (heap_size < k) {
        int pos = heap_size++;
        top[pos] = p;
        while (pos > 0) {
          int parent = (pos - 1) >> 1;
          if (!pair_better(top[parent], top[pos])) break;
          swap_pair(&top[parent], &top[pos]);
          pos = parent;
        }
      } else if (pair_better(p, top[0])) {
        top[0] = p;
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
