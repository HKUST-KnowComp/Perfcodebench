#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static inline void heap_sift_down(Pair* heap, int size, int i) {
  while (1) {
    int smallest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    
    if (left < size) {
      if (heap[left].score < heap[smallest].score ||
          (heap[left].score == heap[smallest].score && heap[left].index > heap[smallest].index)) {
        smallest = left;
      }
    }
    if (right < size) {
      if (heap[right].score < heap[smallest].score ||
          (heap[right].score == heap[smallest].score && heap[right].index > heap[smallest].index)) {
        smallest = right;
      }
    }
    
    if (smallest == i) break;
    
    Pair tmp = heap[i];
    heap[i] = heap[smallest];
    heap[smallest] = tmp;
    i = smallest;
  }
}

static inline void heap_sift_up(Pair* heap, int i) {
  while (i > 0) {
    int parent = (i - 1) / 2;
    if (heap[i].score < heap[parent].score ||
        (heap[i].score == heap[parent].score && heap[i].index > heap[parent].index)) {
      Pair tmp = heap[i];
      heap[i] = heap[parent];
      heap[parent] = tmp;
      i = parent;
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
    // Fill initial heap with first k elements
    for (int i = 0; i < k; ++i) {
      heap[i].score = scores[i];
      heap[i].index = (uint32_t)i;
    }
    
    // Build min-heap (min by score, max by index for ties)
    for (int i = k / 2 - 1; i >= 0; --i) {
      heap_sift_down(heap, k, i);
    }
    
    // Process remaining elements
    for (size_t i = k; i < n; ++i) {
      uint32_t sc = scores[i];
      // Check if current element should replace heap root
      // We want top-k highest scores, so replace if score > min
      // or if score == min but index < root.index (lower index wins)
      if (sc > heap[0].score ||
          (sc == heap[0].score && (uint32_t)i < heap[0].index)) {
        heap[0].score = sc;
        heap[0].index = (uint32_t)i;
        heap_sift_down(heap, k, 0);
      }
    }
    
    // Sort the k winners in descending order
    qsort(heap, k, sizeof(Pair), cmp_pair_desc);
    
    // Compute checksum
    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      h ^= ((uint64_t)heap[i].score << 32) ^ heap[i].index;
      h *= 1099511628211ULL;
    }
  }
  
  free(heap);
  return h;
}