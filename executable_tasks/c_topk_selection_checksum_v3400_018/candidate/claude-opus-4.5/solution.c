#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

/* Min-heap: smallest score at root; ties broken by largest index at root */
static inline int heap_less(const Pair* a, const Pair* b) {
  if (a->score != b->score) return a->score < b->score;
  return a->index > b->index;
}

static inline void heap_sift_down(Pair* h, int n, int i) {
  while (1) {
    int smallest = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;
    if (l < n && heap_less(&h[l], &h[smallest])) smallest = l;
    if (r < n && heap_less(&h[r], &h[smallest])) smallest = r;
    if (smallest == i) break;
    Pair tmp = h[i]; h[i] = h[smallest]; h[smallest] = tmp;
    i = smallest;
  }
}

static inline void heap_sift_up(Pair* h, int i) {
  while (i > 0) {
    int p = (i - 1) / 2;
    if (!heap_less(&h[i], &h[p])) break;
    Pair tmp = h[i]; h[i] = h[p]; h[p] = tmp;
    i = p;
  }
}

/* Max-heap for final extraction: largest score at root; ties broken by smallest index */
static inline int maxheap_less(const Pair* a, const Pair* b) {
  if (a->score != b->score) return a->score > b->score;
  return a->index < b->index;
}

static inline void maxheap_sift_down(Pair* h, int n, int i) {
  while (1) {
    int smallest = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;
    if (l < n && maxheap_less(&h[l], &h[smallest])) smallest = l;
    if (r < n && maxheap_less(&h[r], &h[smallest])) smallest = r;
    if (smallest == i) break;
    Pair tmp = h[i]; h[i] = h[smallest]; h[smallest] = tmp;
    i = smallest;
  }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  if (k <= 0 || n == 0) return 0;
  size_t kk = (size_t)k;
  if (kk > n) kk = n;

  Pair* heap = (Pair*)malloc(kk * sizeof(Pair));
  Pair* result = (Pair*)malloc(kk * sizeof(Pair));
  if (!heap || !result) {
    free(heap);
    free(result);
    return 0;
  }

  uint64_t h = 0;

  for (int iter = 0; iter < iters; ++iter) {
    /* Build min-heap of first k elements */
    for (size_t i = 0; i < kk; ++i) {
      heap[i].score = scores[i];
      heap[i].index = (uint32_t)i;
    }
    /* Heapify */
    for (int i = (int)(kk / 2) - 1; i >= 0; --i) {
      heap_sift_down(heap, (int)kk, i);
    }

    /* Process remaining elements */
    for (size_t i = kk; i < n; ++i) {
      uint32_t sc = scores[i];
      /* Compare with min: if larger score, or same score but smaller index, replace */
      if (sc > heap[0].score || (sc == heap[0].score && (uint32_t)i < heap[0].index)) {
        heap[0].score = sc;
        heap[0].index = (uint32_t)i;
        heap_sift_down(heap, (int)kk, 0);
      }
    }

    /* Extract top-k in descending order using max-heap extraction */
    memcpy(result, heap, kk * sizeof(Pair));
    /* Build max-heap */
    for (int i = (int)(kk / 2) - 1; i >= 0; --i) {
      maxheap_sift_down(result, (int)kk, i);
    }

    /* Compute checksum by extracting in order */
    h = 1469598103934665603ULL;
    for (size_t i = 0; i < kk; ++i) {
      Pair top = result[0];
      h ^= ((uint64_t)top.score << 32) ^ top.index;
      h *= 1099511628211ULL;
      /* Move last to root and sift down */
      int remaining = (int)(kk - i - 1);
      if (remaining > 0) {
        result[0] = result[remaining];
        maxheap_sift_down(result, remaining, 0);
      }
    }
  }

  free(heap);
  free(result);
  return h;
}
