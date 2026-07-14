#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static inline int pair_less(Pair a, Pair b) {
  if (a.score != b.score) return a.score < b.score;
  return a.index > b.index;
}

static inline void heap_sift_down(Pair* h, int n, int i) {
  while (1) {
    int smallest = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;
    if (l < n && pair_less(h[l], h[smallest])) smallest = l;
    if (r < n && pair_less(h[r], h[smallest])) smallest = r;
    if (smallest == i) break;
    Pair tmp = h[i]; h[i] = h[smallest]; h[smallest] = tmp;
    i = smallest;
  }
}

static inline void heap_sift_up(Pair* h, int i) {
  while (i > 0) {
    int p = (i - 1) / 2;
    if (!pair_less(h[i], h[p])) break;
    Pair tmp = h[i]; h[i] = h[p]; h[p] = tmp;
    i = p;
  }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  if (k <= 0 || n == 0) return 0;
  size_t kk = (size_t)k;
  if (kk > n) kk = n;

  Pair* heap = (Pair*)malloc(kk * sizeof(Pair));
  Pair* topk = (Pair*)malloc(kk * sizeof(Pair));
  if (!heap || !topk) { free(heap); free(topk); return 0; }

  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    size_t heap_size = 0;
    for (size_t i = 0; i < n; ++i) {
      Pair cur = { scores[i], (uint32_t)i };
      if (heap_size < kk) {
        heap[heap_size] = cur;
        heap_sift_up(heap, (int)heap_size);
        heap_size++;
      } else if (!pair_less(cur, heap[0])) {
        heap[0] = cur;
        heap_sift_down(heap, (int)kk, 0);
      }
    }
    for (size_t i = 0; i < kk; ++i) topk[i] = heap[i];
    for (int i = (int)kk - 1; i > 0; --i) {
      Pair tmp = topk[0]; topk[0] = topk[i]; topk[i] = tmp;
      heap_sift_down(topk, i, 0);
    }
    h = 1469598103934665603ULL;
    for (size_t i = 0; i < kk; ++i) {
      h ^= ((uint64_t)topk[i].score << 32) ^ topk[i].index;
      h *= 1099511628211ULL;
    }
  }
  free(heap);
  free(topk);
  return h;
}
