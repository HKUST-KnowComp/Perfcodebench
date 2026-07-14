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

static void heap_push(Pair* heap, size_t* heap_size, Pair new_pair) {
  size_t i = *heap_size;
  heap[i] = new_pair;
  (*heap_size)++;
  while (i > 0) {
    size_t parent = (i - 1) / 2;
    if (cmp_pair_desc(&heap[i], &heap[parent]) < 0) {
      Pair temp = heap[parent];
      heap[parent] = heap[i];
      heap[i] = temp;
      i = parent;
    } else {
      break;
    }
  }
}

static void heap_pop(Pair* heap, size_t* heap_size) {
  if (*heap_size == 0) return;
  heap[0] = heap[--(*heap_size)];
  size_t i = 0;
  while (1) {
    size_t left = 2*i + 1;
    size_t right = 2*i + 2;
    size_t smallest = i;
    if (left < *heap_size && cmp_pair_desc(&heap[left], &heap[smallest]) < 0) {
      smallest = left;
    }
    if (right < *heap_size && cmp_pair_desc(&heap[right], &heap[smallest]) < 0) {
      smallest = right;
    }
    if (smallest != i) {
      Pair temp = heap[i];
      heap[i] = heap[smallest];
      heap[smallest] = temp;
      i = smallest;
    } else {
      break;
    }
  }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  if (n == 0 || k <= 0 || iters <= 0) return 0;
  if (k > n) k = (int)n;
  Pair* heap = (Pair*)malloc(k * sizeof(Pair));
  if (!heap) return 0;
  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    size_t heap_size = 0;
    for (size_t i = 0; i < n; ++i) {
      Pair current = {scores[i], (uint32_t)i};
      if (heap_size < k) {
        heap_push(heap, &heap_size, current);
      } else {
        if (cmp_pair_desc(&current, &heap[0]) < 0) {
          heap_pop(heap, &heap_size);
          heap_push(heap, &heap_size, current);
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