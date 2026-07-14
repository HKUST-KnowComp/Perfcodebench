#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

/* Final required order: score descending, index ascending. */
static int better_pair(Pair a, Pair b) {
  if (a.score != b.score) return a.score > b.score;
  return a.index < b.index;
}

/* Heap keeps the current top-k, with the worst element at root.
   "Worse" means lower score, or for equal score larger index. */
static int worse_pair(Pair a, Pair b) {
  if (a.score != b.score) return a.score < b.score;
  return a.index > b.index;
}

static void sift_down_worst(Pair* heap, int size, int idx) {
  for (;;) {
    int left = (idx << 1) + 1;
    if (left >= size) break;
    int right = left + 1;
    int worst = idx;

    if (worse_pair(heap[left], heap[worst])) worst = left;
    if (right < size && worse_pair(heap[right], heap[worst])) worst = right;

    if (worst == idx) break;
    Pair tmp = heap[idx];
    heap[idx] = heap[worst];
    heap[worst] = tmp;
    idx = worst;
  }
}

static void sift_up_worst(Pair* heap, int idx) {
  while (idx > 0) {
    int parent = (idx - 1) >> 1;
    if (!worse_pair(heap[idx], heap[parent])) break;
    Pair tmp = heap[idx];
    heap[idx] = heap[parent];
    heap[parent] = tmp;
    idx = parent;
  }
}

/* Sort winners into exact required order: best first.
   Insertion sort is efficient for small/medium k and has low overhead. */
static void sort_topk(Pair* a, int k) {
  for (int i = 1; i < k; ++i) {
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
  if (k <= 0 || n == 0) {
    uint64_t h = 1469598103934665603ULL;
    return h;
  }

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
        top[heap_size] = p;
        sift_up_worst(top, heap_size);
        ++heap_size;
      } else if (better_pair(p, top[0])) {
        top[0] = p;
        sift_down_worst(top, heap_size, 0);
      }
    }

    sort_topk(top, k);

    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      h ^= ((uint64_t)top[i].score << 32) ^ top[i].index;
      h *= 1099511628211ULL;
    }
  }

  free(top);
  return h;
}
