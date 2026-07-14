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

static inline void heap_replace_root(Pair* h, int n, Pair val) {
  h[0] = val;
  heap_sift_down(h, n, 0);
}

/* Max-heap for final extraction: largest score at root; ties broken by smallest index */
static inline int maxheap_less(const Pair* a, const Pair* b) {
  if (a->score != b->score) return a->score > b->score;
  return a->index < b->index;
}

static inline void maxheap_sift_down(Pair* h, int n, int i) {
  while (1) {
    int largest = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;
    if (l < n && maxheap_less(&h[l], &h[largest])) largest = l;
    if (r < n && maxheap_less(&h[r], &h[largest])) largest = r;
    if (largest == i) break;
    Pair tmp = h[i]; h[i] = h[largest]; h[largest] = tmp;
    i = largest;
  }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  if (k <= 0 || n == 0) return 0;
  if ((size_t)k > n) k = (int)n;

  Pair* heap = (Pair*)malloc((size_t)k * sizeof(Pair));
  Pair* result = (Pair*)malloc((size_t)k * sizeof(Pair));
  if (!heap || !result) {
    free(heap);
    free(result);
    return 0;
  }

  uint64_t h = 0;

  for (int iter = 0; iter < iters; ++iter) {
    /* Build min-heap of first k elements */
    for (int i = 0; i < k; ++i) {
      heap[i].score = scores[i];
      heap[i].index = (uint32_t)i;
    }
    /* Heapify */
    for (int i = k / 2 - 1; i >= 0; --i) {
      heap_sift_down(heap, k, i);
    }

    /* Process remaining elements */
    for (size_t i = (size_t)k; i < n; ++i) {
      uint32_t sc = scores[i];
      /* Check if this element should replace the min */
      /* Element is better if: higher score, or same score with smaller index */
      if (sc > heap[0].score || (sc == heap[0].score && (uint32_t)i < heap[0].index)) {
        Pair p;
        p.score = sc;
        p.index = (uint32_t)i;
        heap_replace_root(heap, k, p);
      }
    }

    /* Copy heap to result and convert to max-heap for extraction in descending order */
    memcpy(result, heap, (size_t)k * sizeof(Pair));
    for (int i = k / 2 - 1; i >= 0; --i) {
      maxheap_sift_down(result, k, i);
    }

    /* Extract in descending order and compute checksum */
    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      Pair top = result[0];
      h ^= ((uint64_t)top.score << 32) ^ top.index;
      h *= 1099511628211ULL;
      /* Move last to root and sift down */
      int remaining = k - i - 1;
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
