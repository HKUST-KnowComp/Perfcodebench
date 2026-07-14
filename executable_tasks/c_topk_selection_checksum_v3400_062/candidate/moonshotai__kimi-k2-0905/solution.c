#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static inline void heap_sift_down(Pair *heap, int root, int k) {
  Pair top = heap[root];
  int child = (root << 1) + 1;
  while (child < k) {
    if (child + 1 < k &&
        (heap[child + 1].score > heap[child].score ||
         (heap[child + 1].score == heap[child].score && heap[child + 1].index < heap[child].index))) {
      child++;
    }
    if (top.score > heap[child].score ||
        (top.score == heap[child].score && top.index <= heap[child].index)) {
      break;
    }
    heap[root] = heap[child];
    root = child;
    child = (root << 1) + 1;
  }
  heap[root] = top;
}

static inline void heap_make(Pair *heap, int k) {
  for (int i = (k >> 1) - 1; i >= 0; --i) {
    heap_sift_down(heap, i, k);
  }
}

static inline void heap_replace_root(Pair *heap, Pair val, int k) {
  heap[0] = val;
  heap_sift_down(heap, 0, k);
}

static inline int cmp_pair_desc(const void *a, const void *b) {
  const Pair *pa = (const Pair *)a;
  const Pair *pb = (const Pair *)b;
  if (pa->score != pb->score) return (pb->score < pa->score) - (pb->score > pa->score);
  return (pa->index < pb->index) - (pa->index > pb->index);
}

uint64_t run(const uint32_t *scores, size_t n, int k, int iters) {
  if (k <= 0) return 0;
  if ((size_t)k > n) k = (int)n;

  Pair *buf = (Pair *)malloc(n * sizeof(Pair));
  if (!buf) return 0;

  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // fill first k
    for (int i = 0; i < k; ++i) {
      buf[i].score = scores[i];
      buf[i].index = (uint32_t)i;
    }
    heap_make(buf, k);

    // process remainder
    for (size_t i = (size_t)k; i < n; ++i) {
      Pair cur;
      cur.score = scores[i];
      cur.index = (uint32_t)i;
      if (cur.score > buf[0].score ||
          (cur.score == buf[0].score && cur.index < buf[0].index)) {
        heap_replace_root(buf, cur, k);
      }
    }

    // final sort of k items
    qsort(buf, k, sizeof(Pair), cmp_pair_desc);

    // hash
    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      h ^= ((uint64_t)buf[i].score << 32) ^ buf[i].index;
      h *= 1099511628211ULL;
    }
  }
  free(buf);
  return h;
}