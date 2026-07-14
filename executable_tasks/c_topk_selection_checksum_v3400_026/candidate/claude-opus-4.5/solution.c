#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

/* Compare for min-heap: smallest score at top, ties broken by largest index */
static inline int heap_less(const Pair* a, const Pair* b) {
  if (a->score != b->score) return a->score < b->score;
  return a->index > b->index;
}

/* Compare for final sort: descending score, ascending index */
static int cmp_pair_desc(const void* lhs, const void* rhs) {
  const Pair* a = (const Pair*) lhs;
  const Pair* b = (const Pair*) rhs;
  if (a->score < b->score) return 1;
  if (a->score > b->score) return -1;
  if (a->index < b->index) return -1;
  if (a->index > b->index) return 1;
  return 0;
}

static inline void sift_down(Pair* heap, int start, int end) {
  int root = start;
  while (1) {
    int child = 2 * root + 1;
    if (child > end) break;
    int swap = root;
    if (heap_less(&heap[child], &heap[swap])) swap = child;
    if (child + 1 <= end && heap_less(&heap[child + 1], &heap[swap])) swap = child + 1;
    if (swap == root) break;
    Pair tmp = heap[root];
    heap[root] = heap[swap];
    heap[swap] = tmp;
    root = swap;
  }
}

static inline void sift_up(Pair* heap, int idx) {
  while (idx > 0) {
    int parent = (idx - 1) / 2;
    if (!heap_less(&heap[idx], &heap[parent])) break;
    Pair tmp = heap[idx];
    heap[idx] = heap[parent];
    heap[parent] = tmp;
    idx = parent;
  }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  if (k <= 0 || n == 0) return 0;
  size_t kk = (size_t)k;
  if (kk > n) kk = n;
  
  Pair* heap = (Pair*)malloc(kk * sizeof(Pair));
  if (!heap) return 0;
  
  uint64_t h = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    /* Build initial heap from first k elements */
    for (size_t i = 0; i < kk; ++i) {
      heap[i].score = scores[i];
      heap[i].index = (uint32_t)i;
    }
    /* Heapify - build min-heap */
    for (int i = (int)(kk / 2) - 1; i >= 0; --i) {
      sift_down(heap, i, (int)kk - 1);
    }
    
    /* Process remaining elements */
    for (size_t i = kk; i < n; ++i) {
      uint32_t sc = scores[i];
      /* Check if current element should replace heap top */
      /* Element is better if: higher score, or same score with lower index */
      if (sc > heap[0].score || (sc == heap[0].score && (uint32_t)i < heap[0].index)) {
        heap[0].score = sc;
        heap[0].index = (uint32_t)i;
        sift_down(heap, 0, (int)kk - 1);
      }
    }
    
    /* Sort the k elements for final ordering */
    qsort(heap, kk, sizeof(Pair), cmp_pair_desc);
    
    /* Compute checksum */
    h = 1469598103934665603ULL;
    for (size_t i = 0; i < kk; ++i) {
      h ^= ((uint64_t)heap[i].score << 32) ^ heap[i].index;
      h *= 1099511628211ULL;
    }
  }
  
  free(heap);
  return h;
}