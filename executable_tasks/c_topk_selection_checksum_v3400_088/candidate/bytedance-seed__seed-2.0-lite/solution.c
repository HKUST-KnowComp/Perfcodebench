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

static inline void sift_up(Pair* heap, size_t pos) {
  while (pos > 0) {
    size_t parent = (pos - 1) / 2;
    if (cmp_pair_desc(&heap[pos], &heap[parent]) < 0) break;
    Pair temp = heap[parent];
    heap[parent] = heap[pos];
    heap[pos] = temp;
    pos = parent;
  }
}

static inline void sift_down(Pair* heap, size_t pos, size_t heap_size) {
  while (1) {
    size_t left = 2 * pos + 1;
    size_t right = 2 * pos + 2;
    size_t smallest = pos;
    if (left < heap_size && cmp_pair_desc(&heap[left], &heap[smallest]) > 0) smallest = left;
    if (right < heap_size && cmp_pair_desc(&heap[right], &heap[smallest]) > 0) smallest = right;
    if (smallest == pos) break;
    Pair temp = heap[pos];
    heap[pos] = heap[smallest];
    heap[smallest] = temp;
    pos = smallest;
  }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  const size_t m = (size_t)k < n ? (size_t)k : n;
  Pair* heap = NULL;
  if (m > 0) {
    heap = (Pair*)malloc(m * sizeof(Pair));
    if (!heap) return 0;
  }
  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    size_t heap_size = 0;
    for (size_t i = 0; i < n; ++i) {
      Pair current = {scores[i], (uint32_t)i};
      if (heap_size < m) {
        heap[heap_size] = current;
        sift_up(heap, heap_size);
        heap_size++;
      } else {
        if (cmp_pair_desc(&current, &heap[0]) < 0) {
          heap[0] = current;
          sift_down(heap, 0, heap_size);
        }
      }
    }
    qsort(heap, m, sizeof(Pair), cmp_pair_desc);
    h = 1469598103934665603ULL;
    for (size_t i = 0; i < m; ++i) {
      h ^= ((uint64_t)heap[i].score << 32) ^ heap[i].index;
      h *= 1099511628211ULL;
    }
  }
  free(heap);
  return h;
}