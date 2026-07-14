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
  if (n == 0 || k <= 0) return 0;
  const int actual_k = (size_t)k > n ? (int)n : k;
  Pair* top_k = (Pair*)malloc(actual_k * sizeof(Pair));
  if (!top_k) return 0;
  
  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    int heap_size = 0;
    
    for (size_t i = 0; i < n; ++i) {
      const uint32_t curr_score = scores[i];
      const uint32_t curr_idx = (uint32_t)i;
      
      if (heap_size < actual_k) {
        top_k[heap_size] = (Pair){curr_score, curr_idx};
        heap_size++;
        
        int child = heap_size - 1;
        while (child > 0) {
          const int parent = (child - 1) / 2;
          const int parent_larger = (top_k[parent].score > top_k[child].score) || 
            (top_k[parent].score == top_k[child].score && top_k[parent].index > top_k[child].index);
          if (parent_larger) {
            const Pair temp = top_k[parent];
            top_k[parent] = top_k[child];
            top_k[child] = temp;
            child = parent;
          } else break;
        }
      } else {
        const int curr_better = (curr_score > top_k[0].score) || 
          (curr_score == top_k[0].score && curr_idx < top_k[0].index);
        if (curr_better) {
          top_k[0] = (Pair){curr_score, curr_idx};
          int parent = 0;
          while (1) {
            const int left = 2 * parent + 1;
            const int right = 2 * parent + 2;
            int smallest = parent;
            
            if (left < heap_size) {
              const int left_smaller = (top_k[left].score < top_k[smallest].score) ||
                (top_k[left].score == top_k[smallest].score && top_k[left].index < top_k[smallest].index);
              if (left_smaller) smallest = left;
            }
            if (right < heap_size) {
              const int right_smaller = (top_k[right].score < top_k[smallest].score) ||
                (top_k[right].score == top_k[smallest].score && top_k[right].index < top_k[smallest].index);
              if (right_smaller) smallest = right;
            }
            if (smallest != parent) {
              const Pair temp = top_k[parent];
              top_k[parent] = top_k[smallest];
              top_k[smallest] = temp;
              parent = smallest;
            } else break;
          }
        }
      }
    }
    
    qsort(top_k, actual_k, sizeof(Pair), cmp_pair_desc);
    h = 1469598103934665603ULL;
    for (int i = 0; i < actual_k; ++i) {
      h ^= ((uint64_t)top_k[i].score << 32) ^ top_k[i].index;
      h *= 1099511628211ULL;
    }
  }
  
  free(top_k);
  return h;
}