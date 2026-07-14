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

static void insertion_sort_topk(Pair* a, int k) {
  for (int i = 1; i < k; ++i) {
    Pair v = a[i];
    int j = i - 1;
    while (j >= 0 && pair_better(v, a[j])) {
      a[j + 1] = a[j];
      --j;
    }
    a[j + 1] = v;
  }
}

static void sift_down_minheap(Pair* heap, int size, int idx) {
  for (;;) {
    int left = idx * 2 + 1;
    if (left >= size) break;
    int right = left + 1;
    int smallest = left;
    if (right < size && pair_better(heap[smallest], heap[right])) smallest = right;
    if (!pair_better(heap[idx], heap[smallest])) break;
    swap_pair(&heap[idx], &heap[smallest]);
    idx = smallest;
  }
}

static void build_minheap(Pair* heap, int size) {
  for (int i = (size >> 1) - 1; i >= 0; --i) sift_down_minheap(heap, size, i);
}

static void quicksort_desc(Pair* a, int left, int right) {
  while (left < right) {
    int i = left;
    int j = right;
    Pair p = a[left + ((right - left) >> 1)];
    do {
      while (pair_better(a[i], p)) ++i;
      while (pair_better(p, a[j])) --j;
      if (i <= j) {
        swap_pair(&a[i], &a[j]);
        ++i;
        --j;
      }
    } while (i <= j);

    if (j - left < right - i) {
      if (left < j) quicksort_desc(a, left, j);
      left = i;
    } else {
      if (i < right) quicksort_desc(a, i, right);
      right = j;
    }
  }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  if (!scores || n == 0 || k <= 0 || iters <= 0) return 1469598103934665603ULL;
  if ((size_t)k > n) k = (int)n;

  Pair* top = (Pair*)malloc((size_t)k * sizeof(Pair));
  if (!top) return 0;

  uint64_t h = 1469598103934665603ULL;

  for (int iter = 0; iter < iters; ++iter) {
    for (int i = 0; i < k; ++i) {
      top[i].score = scores[i];
      top[i].index = (uint32_t)i;
    }

    build_minheap(top, k);

    for (size_t i = (size_t)k; i < n; ++i) {
      Pair cand;
      cand.score = scores[i];
      cand.index = (uint32_t)i;
      if (pair_better(cand, top[0])) {
        top[0] = cand;
        sift_down_minheap(top, k, 0);
      }
    }

    if (k <= 32) {
      insertion_sort_topk(top, k);
    } else {
      quicksort_desc(top, 0, k - 1);
    }

    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      h ^= ((uint64_t)top[i].score << 32) ^ top[i].index;
      h *= 1099511628211ULL;
    }
  }

  free(top);
  return h;
}
