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

static inline int is_better(const Pair* a, const Pair* b) {
  if (a->score > b->score) return 1;
  if (a->score == b->score && a->index < b->index) return 1;
  return 0;
}

static void sift_up(Pair* heap, int pos) {
  while (pos > 0) {
    int parent = (pos - 1) / 2;
    if (is_better(&heap[pos], &heap[parent])) {
      Pair temp = heap[parent];
      heap[parent] = heap[pos];
      heap[pos] = temp;
      pos = parent;
    } else {
      break;
    }
  }
}

static void sift_down(Pair* heap, int heap_size, int pos) {
  while (1) {
    int left = 2 * pos + 1;
    int right = 2 * pos + 2;
    int smallest = pos;
    if (left < heap_size && is_better(&heap[smallest], &heap[left])) {
      smallest = left;
    }
    if (right < heap_size && is_better(&heap[smallest], &heap[right])) {
      smallest = right;
    }
    if (smallest != pos) {
      Pair temp = heap[pos];
      heap[pos] = heap[smallest];
      heap[smallest] = temp;
      pos = smallest;
    } else {
      break;
    }
  }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  if (k <= 0) return 1469598103934665603ULL;
  Pair* pairs = (Pair*)malloc(n * sizeof(Pair));
  Pair* heap = (Pair*)malloc(k * sizeof(Pair));
  if (!pairs || !heap) {
    free(pairs);
    free(heap);
    return 0;
  }
  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    int heap_size = 0;
    for (size_t i = 0; i < n; ++i) {
      Pair current = {scores[i], (uint32_t)i};
      if (heap_size < k) {
        heap[heap_size] = current;
        sift_up(heap, heap_size);
        heap_size++;
      } else if (is_better(&current, &heap[0])) {
        heap[0] = current;
        sift_down(heap, k, 0);
      }
    }
    for (int i = 0; i < k; ++i) {
      pairs[i] = heap[i];
    }
    qsort(pairs, k, sizeof(Pair), cmp_pair_desc);
    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
      h *= 1099511628211ULL;
    }
  }
  free(pairs);
  free(heap);
  return h;
}