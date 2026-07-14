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

static void sift_up(Pair* heap, int idx, int size) {
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (cmp_pair_desc(&heap[parent], &heap[idx]) > 0) {
            Pair temp = heap[parent];
            heap[parent] = heap[idx];
            heap[idx] = temp;
            idx = parent;
        } else {
            break;
        }
    }
}

static void sift_down(Pair* heap, int idx, int size) {
    while (1) {
        int left = 2 * idx + 1;
        int right = 2 * idx + 2;
        int smallest = idx;

        if (left < size && cmp_pair_desc(&heap[smallest], &heap[left]) > 0) {
            smallest = left;
        }
        if (right < size && cmp_pair_desc(&heap[smallest], &heap[right]) > 0) {
            smallest = right;
        }

        if (smallest != idx) {
            Pair temp = heap[idx];
            heap[idx] = heap[smallest];
            heap[smallest] = temp;
            idx = smallest;
        } else {
            break;
        }
    }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  Pair* heap = (Pair*)malloc(k * sizeof(Pair));
  if (!heap) return 0;
  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    int heap_size = 0;
    for (size_t i = 0; i < n; ++i) {
      Pair current = {scores[i], (uint32_t)i};
      if (heap_size < k) {
        heap[heap_size] = current;
        sift_up(heap, heap_size, k);
        heap_size++;
      } else {
        if (cmp_pair_desc(&heap[0], &current) > 0) {
          heap[0] = current;
          sift_down(heap, 0, k);
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