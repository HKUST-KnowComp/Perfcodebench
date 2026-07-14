#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static inline int is_worse(Pair a, Pair b) {
    if (a.score != b.score) return a.score < b.score;
    return a.index > b.index;
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
  if (k <= 0) return 0;
  if ((size_t)k > n) k = (int)n;
  
  Pair* heap = (Pair*)malloc(k * sizeof(Pair));
  if (!heap) return 0;
  
  uint64_t h = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Fill heap with first k elements
    for (int i = 0; i < k; ++i) {
      heap[i].score = scores[i];
      heap[i].index = (uint32_t)i;
      int idx = i;
      while (idx > 0) {
        int parent = (idx - 1) >> 1;
        if (is_worse(heap[idx], heap[parent])) {
          Pair tmp = heap[parent];
          heap[parent] = heap[idx];
          heap[idx] = tmp;
          idx = parent;
        } else {
          break;
        }
      }
    }
    
    // Process remaining elements
    for (size_t i = k; i < n; ++i) {
      uint32_t s = scores[i];
      uint32_t idx_val = (uint32_t)i;
      
      // If current is better than the worst in heap (root)
      if (s > heap[0].score || (s == heap[0].score && idx_val < heap[0].index)) {
        heap[0].score = s;
        heap[0].index = idx_val;
        
        int idx = 0;
        while (1) {
          int left = (idx << 1) + 1;
          int right = left + 1;
          int smallest = idx;
          if (left < k && is_worse(heap[left], heap[smallest])) smallest = left;
          if (right < k && is_worse(heap[right], heap[smallest])) smallest = right;
          if (smallest != idx) {
            Pair tmp = heap[idx];
            heap[idx] = heap[smallest];
            heap[smallest] = tmp;
            idx = smallest;
          } else {
            break;
          }
        }
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