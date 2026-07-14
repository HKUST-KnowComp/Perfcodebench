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

static inline void sift_down(Pair* heap, int heap_size, int pos) {
  while (1) {
    int left = 2 * pos + 1;
    int right = 2 * pos + 2;
    int smallest = pos;
    if (left < heap_size && cmp_pair_desc(&heap[left], &heap[smallest]) < 0) {
      smallest = left;
    }
    if (right < heap_size && cmp_pair_desc(&heap[right], &heap[smallest]) < 0) {
      smallest = right;
    }
    if (smallest == pos) break;
    Pair temp = heap[pos];
    heap[pos] = heap[smallest];
    heap[smallest] = temp;
    pos = smallest;
  }
}

static inline void sift_up(Pair* heap, int pos) {
  while (pos > 0) {
    int parent = (pos - 1) / 2;
    if (cmp_pair_desc(&heap[pos], &heap[parent]) < 0) {
      Pair temp = heap[parent];
      heap[parent] = heap[pos];
      heap[pos] = temp;
      pos = parent;
    } else {
      break;
    }
  }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  if (n == 0 || k <= 0 || iters <= 0) return 0;
  if ((size_t)k > n) k = (int)n;
  
  Pair* top_pairs = (Pair*)malloc(k * sizeof(Pair));
  if (!top_pairs) return 0;
  
  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    int heap_size = 0;
    for (size_t i = 0; i < n; ++i) {
      Pair current = {scores[i], (uint32_t)i};
      if (heap_size < k) {
        top_pairs[heap_size] = current;
        sift_up(top_pairs, heap_size);
        heap_size++;
      } else {
        if (cmp_pair_desc(&current, top_pairs) < 0) {
          top_pairs[0] = current;
          sift_down(top_pairs, k, 0);
        }
      }
    }
    qsort(top_pairs, k, sizeof(Pair), cmp_pair_desc);
    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      h ^= ((uint64_t)top_pairs[i].score << 32) ^ top_pairs[i].index;
      h *= 1099511628211ULL;
    }
  }
  free(top_pairs);
  return h;
}