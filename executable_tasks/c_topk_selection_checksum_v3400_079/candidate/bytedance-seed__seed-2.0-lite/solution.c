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

static void sift_down(Pair* heap, size_t heap_size, size_t root) {
  while (1) {
    size_t left = 2 * root + 1;
    if (left >= heap_size) break;
    size_t smallest = root;
    if (cmp_pair_desc(&heap[left], &heap[smallest]) > 0) {
      smallest = left;
    }
    size_t right = left + 1;
    if (right < heap_size && cmp_pair_desc(&heap[right], &heap[smallest]) > 0) {
      smallest = right;
    }
    if (smallest == root) break;
    Pair temp = heap[root];
    heap[root] = heap[smallest];
    heap[smallest] = temp;
    root = smallest;
  }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  if (n == 0 || k <= 0) return 0;
  const size_t actual_k = (size_t)k < n ? (size_t)k : n;
  Pair* heap = (Pair*)malloc(actual_k * sizeof(Pair));
  if (!heap) return 0;
  uint64_t h = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    size_t heap_ptr = 0;
    for (size_t i = 0; i < n; ++i) {
      Pair curr = {scores[i], (uint32_t)i};
      if (heap_ptr < actual_k) {
        heap[heap_ptr] = curr;
        heap_ptr++;
        if (heap_ptr == actual_k) {
          for (int j = (actual_k / 2) - 1; j >= 0; j--) {
            sift_down(heap, actual_k, j);
          }
        }
      } else {
        if (cmp_pair_desc(&curr, &heap[0]) < 0) {
          heap[0] = curr;
          sift_down(heap, actual_k, 0);
        }
      }
    }
    qsort(heap, actual_k, sizeof(Pair), cmp_pair_desc);
    h = 1469598103934665603ULL;
    for (size_t i = 0; i < actual_k; ++i) {
      h ^= ((uint64_t)heap[i].score << 32) ^ heap[i].index;
      h *= 1099511628211ULL;
    }
  }
  free(heap);
  return h;
}