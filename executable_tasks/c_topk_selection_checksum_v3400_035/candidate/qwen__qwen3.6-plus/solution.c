#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

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
  if (k > (int)n) k = (int)n;
  
  Pair* heap = (Pair*)malloc(k * sizeof(Pair));
  if (!heap) return 0;
  
  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    int heap_size = 0;
    for (size_t i = 0; i < n; ++i) {
      uint32_t s = scores[i];
      uint32_t idx = (uint32_t)i;
      
      if (heap_size < k) {
        heap[heap_size].score = s;
        heap[heap_size].index = idx;
        int cur = heap_size;
        while (cur > 0) {
          int parent = (cur - 1) >> 1;
          if (heap[cur].score < heap[parent].score || 
             (heap[cur].score == heap[parent].score && heap[cur].index > heap[parent].index)) {
            Pair tmp = heap[cur]; heap[cur] = heap[parent]; heap[parent] = tmp;
            cur = parent;
          } else {
            break;
          }
        }
        heap_size++;
      } else {
        if (s > heap[0].score || (s == heap[0].score && idx < heap[0].index)) {
          heap[0].score = s;
          heap[0].index = idx;
          int cur = 0;
          while (1) {
            int smallest = cur;
            int left = (cur << 1) + 1;
            int right = left + 1;
            if (left < k) {
              if (heap[left].score < heap[smallest].score || 
                 (heap[left].score == heap[smallest].score && heap[left].index > heap[smallest].index)) {
                smallest = left;
              }
            }
            if (right < k) {
              if (heap[right].score < heap[smallest].score || 
                 (heap[right].score == heap[smallest].score && heap[right].index > heap[smallest].index)) {
                smallest = right;
              }
            }
            if (smallest != cur) {
              Pair tmp = heap[cur]; heap[cur] = heap[smallest]; heap[smallest] = tmp;
              cur = smallest;
            } else {
              break;
            }
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