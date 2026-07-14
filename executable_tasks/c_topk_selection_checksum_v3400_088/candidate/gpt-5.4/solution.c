#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

/*
  Final required order:
    - higher score first
    - for equal score, lower index first

  For maintaining the current top-k, we use a min-heap whose root is the
  worst element among the kept winners:
    - lower score is worse
    - for equal score, higher index is worse
*/

static int better_pair(Pair a, Pair b) {
  if (a.score != b.score) return a.score > b.score;
  return a.index < b.index;
}

static int worse_pair(Pair a, Pair b) {
  if (a.score != b.score) return a.score < b.score;
  return a.index > b.index;
}

static void heap_sift_up(Pair* heap, int idx) {
  while (idx > 0) {
    int parent = (idx - 1) >> 1;
    if (!worse_pair(heap[idx], heap[parent])) break;
    Pair tmp = heap[idx];
    heap[idx] = heap[parent];
    heap[parent] = tmp;
    idx = parent;
  }
}

static void heap_sift_down(Pair* heap, int size, int idx) {
  for (;;) {
    int left = (idx << 1) + 1;
    if (left >= size) break;
    int right = left + 1;
    int smallest = left;
    if (right < size && worse_pair(heap[right], heap[left])) smallest = right;
    if (!worse_pair(heap[smallest], heap[idx])) break;
    Pair tmp = heap[idx];
    heap[idx] = heap[smallest];
    heap[smallest] = tmp;
    idx = smallest;
  }
}

static void sort_desc(Pair* a, int n) {
  for (int i = 1; i < n; ++i) {
    Pair v = a[i];
    int j = i - 1;
    while (j >= 0 && better_pair(v, a[j])) {
      a[j + 1] = a[j];
      --j;
    }
    a[j + 1] = v;
  }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  if (iters <= 0) return 0;
  if (k <= 0) {
    uint64_t h = 0;
    for (int iter = 0; iter < iters; ++iter) {
      h = 1469598103934665603ULL;
    }
    return h;
  }

  size_t kk = (size_t)k;
  if (kk > n) kk = n;
  if (kk == 0) {
    uint64_t h = 0;
    for (int iter = 0; iter < iters; ++iter) {
      h = 1469598103934665603ULL;
    }
    return h;
  }

  Pair* heap = (Pair*)malloc(kk * sizeof(Pair));
  if (!heap) return 0;

  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    int hs = 0;

    size_t i = 0;
    for (; i < kk; ++i) {
      Pair p;
      p.score = scores[i];
      p.index = (uint32_t)i;
      heap[hs] = p;
      heap_sift_up(heap, hs);
      ++hs;
    }

    for (; i < n; ++i) {
      Pair p;
      p.score = scores[i];
      p.index = (uint32_t)i;
      if (better_pair(p, heap[0])) {
        heap[0] = p;
        heap_sift_down(heap, hs, 0);
      }
    }

    sort_desc(heap, hs);

    h = 1469598103934665603ULL;
    for (int j = 0; j < hs; ++j) {
      h ^= ((uint64_t)heap[j].score << 32) ^ heap[j].index;
      h *= 1099511628211ULL;
    }
  }

  free(heap);
  return h;
}
