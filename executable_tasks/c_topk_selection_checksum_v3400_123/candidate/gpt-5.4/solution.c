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

static void insertion_sort_desc(Pair* a, int n) {
  for (int i = 1; i < n; ++i) {
    Pair v = a[i];
    int j = i - 1;
    while (j >= 0 && pair_better(v, a[j])) {
      a[j + 1] = a[j];
      --j;
    }
    a[j + 1] = v;
  }
}

static void sift_down(Pair* heap, int size, int idx) {
  Pair v = heap[idx];
  while (1) {
    int left = idx * 2 + 1;
    if (left >= size) break;
    int right = left + 1;
    int child = left;
    if (right < size && pair_better(heap[right], heap[left])) child = right;
    if (!pair_better(heap[child], v)) break;
    heap[idx] = heap[child];
    idx = child;
  }
  heap[idx] = v;
}

static void heap_sort_desc(Pair* a, int n) {
  if (n <= 1) return;
  for (int i = n / 2 - 1; i >= 0; --i) sift_down(a, n, i);
  for (int end = n - 1; end > 0; --end) {
    swap_pair(&a[0], &a[end]);
    sift_down(a, end, 0);
  }
  for (int i = 0, j = n - 1; i < j; ++i, --j) swap_pair(&a[i], &a[j]);
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  if (k <= 0 || n == 0 || iters <= 0) return 1469598103934665603ULL;
  if ((size_t)k > n) k = (int)n;

  Pair* top = (Pair*)malloc((size_t)k * sizeof(Pair));
  if (!top) return 0;

  uint64_t h = 1469598103934665603ULL;

  for (int iter = 0; iter < iters; ++iter) {
    int heap_size = 0;

    for (size_t i = 0; i < n; ++i) {
      Pair p;
      p.score = scores[i];
      p.index = (uint32_t)i;

      if (heap_size < k) {
        int pos = heap_size++;
        while (pos > 0) {
          int parent = (pos - 1) >> 1;
          if (!pair_better(top[parent], p)) break;
          top[pos] = top[parent];
          pos = parent;
        }
        top[pos] = p;
      } else {
        Pair root = top[0];
        if (pair_better(p, root)) {
          int pos = 0;
          while (1) {
            int left = pos * 2 + 1;
            if (left >= k) break;
            int right = left + 1;
            int child = left;
            if (right < k && pair_better(top[left], top[right])) child = right;
            if (!pair_better(top[child], p)) break;
            top[pos] = top[child];
            pos = child;
          }
          top[pos] = p;
        }
      }
    }

    if (k <= 32) insertion_sort_desc(top, k);
    else heap_sort_desc(top, k);

    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      h ^= ((uint64_t)top[i].score << 32) ^ top[i].index;
      h *= 1099511628211ULL;
    }
  }

  free(top);
  return h;
}
