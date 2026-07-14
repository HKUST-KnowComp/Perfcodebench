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

static inline void heap_swap(Pair* restrict a, Pair* restrict b) {
  Pair t = *a;
  *a = *b;
  *b = t;
}

static inline int is_better(const Pair* candidate, const Pair* current) {
  return cmp_pair_desc(candidate, current) < 0;
}

static void heapify_down(Pair* heap, int heap_size, int parent_idx) {
  while (1) {
    int left_idx = 2 * parent_idx + 1;
    int right_idx = 2 * parent_idx + 2;
    int smallest_idx = parent_idx;
    if (left_idx < heap_size && is_better(&heap[left_idx], &heap[smallest_idx])) {
      smallest_idx = left_idx;
    }
    if (right_idx < heap_size && is_better(&heap[right_idx], &heap[smallest_idx])) {
      smallest_idx = right_idx;
    }
    if (smallest_idx == parent_idx) break;
    heap_swap(&heap[parent_idx], &heap[smallest_idx]);
    parent_idx = smallest_idx;
  }
}

static void heapify_up(Pair* heap, int child_idx) {
  while (child_idx > 0) {
    int parent_idx = (child_idx - 1) / 2;
    if (is_better(&heap[child_idx], &heap[parent_idx])) {
      heap_swap(&heap[child_idx], &heap[parent_idx]);
      child_idx = parent_idx;
    } else break;
  }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  if (n == 0 || k <= 0 || iters <=0) return 1469598103934665603ULL;
  const int actual_k = (size_t)k > n ? (int)n : k;
  Pair* pairs = (Pair*)malloc(n * sizeof(Pair));
  Pair* top_heap = (Pair*)malloc(actual_k * sizeof(Pair));
  if (!pairs || !top_heap) {
    free(pairs); free(top_heap);
    return 0;
  }
  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (size_t i = 0; i < n; ++i) {
      pairs[i].score = scores[i];
      pairs[i].index = (uint32_t)i;
    }
    int heap_len = 0;
    for (size_t i = 0; i < n; ++i) {
      const Pair curr = pairs[i];
      if (heap_len < actual_k) {
        top_heap[heap_len] = curr;
        heapify_up(top_heap, heap_len);
        heap_len++;
      } else if (is_better(&curr, &top_heap[0])) {
        top_heap[0] = curr;
        heapify_down(top_heap, actual_k, 0);
      }
    }
    qsort(top_heap, actual_k, sizeof(Pair), cmp_pair_desc);
    h = 1469598103934665603ULL;
    for (int i = 0; i < k && i < actual_k; ++i) {
      h ^= ((uint64_t)top_heap[i].score << 32) ^ top_heap[i].index;
      h *= 1099511628211ULL;
    }
  }
  free(pairs);
  free(top_heap);
  return h;
}