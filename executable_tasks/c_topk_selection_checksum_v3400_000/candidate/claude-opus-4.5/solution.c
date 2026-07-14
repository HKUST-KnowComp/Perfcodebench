#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static inline int pair_less(const Pair* a, const Pair* b) {
  if (a->score != b->score) return a->score < b->score;
  return a->index > b->index;
}

static inline void heap_sift_down(Pair* heap, int n, int i) {
  while (1) {
    int smallest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    if (left < n && pair_less(&heap[left], &heap[smallest])) smallest = left;
    if (right < n && pair_less(&heap[right], &heap[smallest])) smallest = right;
    if (smallest == i) break;
    Pair tmp = heap[i]; heap[i] = heap[smallest]; heap[smallest] = tmp;
    i = smallest;
  }
}

static inline void heap_sift_up(Pair* heap, int i) {
  while (i > 0) {
    int parent = (i - 1) / 2;
    if (!pair_less(&heap[i], &heap[parent])) break;
    Pair tmp = heap[i]; heap[i] = heap[parent]; heap[parent] = tmp;
    i = parent;
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
  size_t kk = (size_t)k;
  if (kk > n) kk = n;
  
  Pair* heap = (Pair*)malloc(kk * sizeof(Pair));
  if (!heap) return 0;
  
  uint64_t h = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    size_t heap_size = 0;
    
    for (size_t i = 0; i < n; ++i) {
      uint32_t sc = scores[i];
      if (heap_size < kk) {
        heap[heap_size].score = sc;
        heap[heap_size].index = (uint32_t)i;
        heap_sift_up(heap, (int)heap_size);
        heap_size++;
      } else if (sc > heap[0].score || (sc == heap[0].score && (uint32_t)i < heap[0].index)) {
        heap[0].score = sc;
        heap[0].index = (uint32_t)i;
        heap_sift_down(heap, (int)kk, 0);
      }
    }
    
    qsort(heap, kk, sizeof(Pair), cmp_pair_desc);
    
    h = 1469598103934665603ULL;
    for (size_t i = 0; i < kk; ++i) {
      h ^= ((uint64_t)heap[i].score << 32) ^ heap[i].index;
      h *= 1099511628211ULL;
    }
  }
  
  free(heap);
  return h;
}