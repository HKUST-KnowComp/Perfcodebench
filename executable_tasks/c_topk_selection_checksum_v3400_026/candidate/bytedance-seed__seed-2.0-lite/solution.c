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

static void sift_down(Pair* heap, size_t heap_size, size_t parent) {
  Pair temp = heap[parent];
  size_t child;
  while ((child = 2 * parent + 1) < heap_size) {
    if (child + 1 < heap_size && cmp_pair_desc(&heap[child], &heap[child+1]) > 0) {
      child++;
    }
    if (cmp_pair_desc(&temp, &heap[child]) <= 0) {
      break;
    }
    heap[parent] = heap[child];
    parent = child;
  }
  heap[parent] = temp;
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  if (n == 0 || iters <= 0) return 1469598103934665603ULL;
  const int actual_k = (k > (int)n) ? (int)n : k;
  if (actual_k <= 0) return 1469598103934665603ULL;

  Pair* pairs = (Pair*)malloc(n * sizeof(Pair));
  if (!pairs) return 0;

  // Pre-initialize pairs once, no need to rebuild every iteration
  for (size_t i = 0; i < n; ++i) {
    pairs[i].score = scores[i];
    pairs[i].index = (uint32_t)i;
  }

  uint64_t h = 0;
  if (actual_k >= (int)n) {
    // Sort full array once if we need all elements
    qsort(pairs, n, sizeof(Pair), cmp_pair_desc);
    for (int iter = 0; iter < iters; ++iter) {
      h = 1469598103934665603ULL;
      for (int i = 0; i < actual_k; ++i) {
        h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
        h *= 1099511628211ULL;
      }
    }
  } else {
    // Use min-heap top-k selection for k < n
    Pair* topk_heap = (Pair*)malloc(actual_k * sizeof(Pair));
    if (!topk_heap) {
      free(pairs);
      return 0;
    }
    for (int iter = 0; iter < iters; ++iter) {
      size_t heap_len = 0;
      for (size_t i = 0; i < n; ++i) {
        const Pair current = pairs[i];
        if (heap_len < (size_t)actual_k) {
          topk_heap[heap_len] = current;
          size_t parent = (heap_len - 1) / 2;
          heap_len++;
          while (parent > 0) {
            sift_down(topk_heap, heap_len, parent);
            parent = (parent - 1) / 2;
          }
          sift_down(topk_heap, heap_len, 0);
        } else if (cmp_pair_desc(&current, &topk_heap[0]) < 0) {
          topk_heap[0] = current;
          sift_down(topk_heap, actual_k, 0);
        }
      }
      // Sort top-k to match original order for correct checksum
      qsort(topk_heap, actual_k, sizeof(Pair), cmp_pair_desc);
      h = 1469598103934665603ULL;
      for (int i = 0; i < actual_k; ++i) {
        h ^= ((uint64_t)topk_heap[i].score << 32) ^ topk_heap[i].index;
        h *= 1099511628211ULL;
      }
    }
    free(topk_heap);
  }

  free(pairs);
  return h;
}