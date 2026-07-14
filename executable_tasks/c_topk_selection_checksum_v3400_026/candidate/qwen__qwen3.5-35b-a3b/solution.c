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

static void heap_insert(Pair* heap, int* size, Pair new_elem) {
    int i = *size;
    heap[i] = new_elem;
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (cmp_pair_desc(&heap[parent], &heap[i]) < 0) {
            Pair temp = heap[parent];
            heap[parent] = heap[i];
            heap[i] = temp;
            i = parent;
        } else {
            break;
        }
    }
    (*size)++;
}

static void heap_sift_down(Pair* heap, int size, int index) {
    while (2 * index + 1 < size) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int smallest = index;

        if (left < size && cmp_pair_desc(&heap[index], &heap[left]) > 0) {
            smallest = left;
        }
        if (right < size && cmp_pair_desc(&heap[smallest], &heap[right]) > 0) {
            smallest = right;
        }

        if (smallest != index) {
            Pair temp = heap[index];
            heap[index] = heap[smallest];
            heap[smallest] = temp;
            index = smallest;
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
      Pair p = {scores[i], (uint32_t)i};
      if (heap_size < k) {
        heap_insert(heap, &heap_size, p);
      } else {
        if (cmp_pair_desc(&heap[0], &p) > 0) {
          heap[0] = p;
          heap_sift_down(heap, k, 0);
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