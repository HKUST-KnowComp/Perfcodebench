#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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

static inline int is_less(const Pair* a, const Pair* b) {
  if (a->score != b->score) return a->score < b->score;
  return a->index > b->index;
}

static inline void sift_up(Pair* heap, size_t pos) {
  while (pos > 0) {
    size_t parent = (pos - 1) / 2;
    if (!is_less(&heap[pos], &heap[parent])) break;
    Pair temp = heap[parent];
    heap[parent] = heap[pos];
    heap[pos] = temp;
    pos = parent;
  }
}

static inline void sift_down(Pair* heap, size_t heap_size) {
  size_t pos = 0;
  while (1) {
    size_t left = 2 * pos + 1;
    size_t right = 2 * pos + 2;
    size_t smallest = pos;
    if (left < heap_size && is_less(&heap[left], &heap[smallest])) smallest = left;
    if (right < heap_size && is_less(&heap[right], &heap[smallest])) smallest = right;
    if (smallest == pos) break;
    Pair temp = heap[pos];
    heap[pos] = heap[smallest];
    heap[smallest] = temp;
    pos = smallest;
  }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  Pair* pairs = (Pair*)malloc(n * sizeof(Pair));
  if (!pairs) return 0;
  Pair* heap = NULL;
  int actual_k = k > (int)n ? (int)n : k;
  if (actual_k > 0) {
    heap = (Pair*)malloc(actual_k * sizeof(Pair));
    if (!heap) {
      free(pairs);
      return 0;
    }
  }
  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (size_t i = 0; i < n; ++i) {
      pairs[i].score = scores[i];
      pairs[i].index = (uint32_t)i;
    }
    if (actual_k <= 0) {
      h = 1469598103934665603ULL;
      continue;
    }
    size_t heap_size = 0;
    for (size_t i = 0; i < n; ++i) {
      const Pair* current = &pairs[i];
      if (heap_size < (size_t)actual_k) {
        heap[heap_size] = *current;
        sift_up(heap, heap_size);
        heap_size++;
      } else if (is_less(&heap[0], current)) {
        heap[0] = *current;
        sift_down(heap, heap_size);
      }
    }
    qsort(heap, heap_size, sizeof(Pair), cmp_pair_desc);
    h = 1469598103934665603ULL;
    for (int i = 0; i < actual_k; ++i) {
      h ^= ((uint64_t)heap[i].score << 32) ^ heap[i].index;
      h *= 1099511628211ULL;
    }
  }
  free(heap);
  free(pairs);
  return h;
}