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

static inline int is_less(const Pair* a, const Pair* b) {
  return cmp_pair_desc(a, b) > 0;
}

static void heapify_up(Pair* heap, int pos) {
  while (pos > 0) {
    int parent = (pos - 1) / 2;
    if (is_less(&heap[pos], &heap[parent])) break;
    Pair temp = heap[parent];
    heap[parent] = heap[pos];
    heap[pos] = temp;
    pos = parent;
  }
}

static void heapify_down(Pair* heap, int heap_size, int pos) {
  while (1) {
    int left = 2 * pos + 1;
    int right = 2 * pos + 2;
    int smallest = pos;
    if (left < heap_size && is_less(&heap[smallest], &heap[left])) smallest = left;
    if (right < heap_size && is_less(&heap[smallest], &heap[right])) smallest = right;
    if (smallest == pos) break;
    Pair temp = heap[pos];
    heap[pos] = heap[smallest];
    heap[smallest] = temp;
    pos = smallest;
  }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  if (k <= 0) return 1469598103934665603ULL;
  const size_t actual_k = (size_t)k < n ? (size_t)k : n;
  Pair* pairs = (Pair*)malloc(n * sizeof(Pair));
  Pair* topk_heap = (Pair*)malloc(actual_k * sizeof(Pair));
  if (!pairs || !topk_heap) { free(pairs); free(topk_heap); return 0; }
  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (size_t i = 0; i < n; ++i) {
      pairs[i].score = scores[i];
      pairs[i].index = (uint32_t)i;
    }
    int heap_size = 0;
    for (size_t i = 0; i < n; ++i) {
      const Pair current = pairs[i];
      if (heap_size < actual_k) {
        topk_heap[heap_size] = current;
        heapify_up(topk_heap, heap_size);
        heap_size++;
      } else if (is_less(&topk_heap[0], &current)) {
        topk_heap[0] = current;
        heapify_down(topk_heap, actual_k, 0);
      }
    }
    qsort(topk_heap, actual_k, sizeof(Pair), cmp_pair_desc);
    h = 1469598103934665603ULL;
    for (size_t i = 0; i < actual_k; ++i) {
      h ^= ((uint64_t)topk_heap[i].score << 32) ^ topk_heap[i].index;
      h *= 1099511628211ULL;
    }
  }
  free(pairs);
  free(topk_heap);
  return h;
}