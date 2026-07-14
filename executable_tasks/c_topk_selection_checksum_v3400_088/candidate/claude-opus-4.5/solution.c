#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static inline void heap_sift_down_min(Pair* h, int n, int i) {
  while (1) {
    int smallest = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;
    if (l < n && (h[l].score < h[smallest].score ||
        (h[l].score == h[smallest].score && h[l].index > h[smallest].index))) {
      smallest = l;
    }
    if (r < n && (h[r].score < h[smallest].score ||
        (h[r].score == h[smallest].score && h[r].index > h[smallest].index))) {
      smallest = r;
    }
    if (smallest == i) break;
    Pair tmp = h[i];
    h[i] = h[smallest];
    h[smallest] = tmp;
    i = smallest;
  }
}

static inline void heap_sift_up_min(Pair* h, int i) {
  while (i > 0) {
    int p = (i - 1) / 2;
    if (h[i].score < h[p].score ||
        (h[i].score == h[p].score && h[i].index > h[p].index)) {
      Pair tmp = h[i];
      h[i] = h[p];
      h[p] = tmp;
      i = p;
    } else {
      break;
    }
  }
}

static int cmp_pair_desc(const void* lhs, const void* rhs) {
  const Pair* a = (const Pair*) lhs;
  const Pair* b = (const Pair*) rhs;
  if (a->score < b->score) return 1;
  if (a->score > b->score) return -1;
  if (a->index < b->index) return -1;
  if (a->index > b->index) return 1;
  return 0;
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  if (k <= 0 || n == 0) return 0;
  if ((size_t)k > n) k = (int)n;
  
  Pair* heap = (Pair*)malloc(k * sizeof(Pair));
  if (!heap) return 0;
  
  uint64_t h = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    int heap_size = 0;
    
    for (size_t i = 0; i < (size_t)k; ++i) {
      heap[heap_size].score = scores[i];
      heap[heap_size].index = (uint32_t)i;
      heap_sift_up_min(heap, heap_size);
      heap_size++;
    }
    
    for (size_t i = k; i < n; ++i) {
      uint32_t sc = scores[i];
      if (sc > heap[0].score ||
          (sc == heap[0].score && (uint32_t)i < heap[0].index)) {
        heap[0].score = sc;
        heap[0].index = (uint32_t)i;
        heap_sift_down_min(heap, k, 0);
      }
    }
    
    qsort(heap, k, sizeof(Pair), cmp_pair_desc);
    
    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      h ^= ((uint64_t)heap[i].score << 32) ^ heap[i].index;
      h *= 1099511628211ULL;
    }
  }
  
  free(heap);
  return h;
}
