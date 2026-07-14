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

static void heap_push(Pair* heap, int* size, Pair new_elem) {
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

static void heap_replace_root(Pair* heap, int size, Pair new_elem) {
    heap[0] = new_elem;
    int i = 0;
    while (1) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int smallest = i;

        if (left < size && cmp_pair_desc(&heap[smallest], &heap[left]) > 0) {
            smallest = left;
        }
        if (right < size && cmp_pair_desc(&heap[smallest], &heap[right]) > 0) {
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
    if (k <= 0 || k > n) return 0;

    Pair* heap = (Pair*)malloc(k * sizeof(Pair));
    if (!heap) return 0;

    uint64_t h = 0;
    for (int iter = 0; iter < iters; ++iter) {
        int heap_size = 0;
        for (size_t i = 0; i < n; ++i) {
            Pair new_elem = {scores[i], (uint32_t)i};
            if (heap_size < k) {
                heap_push(heap, &heap_size, new_elem);
            } else if (cmp_pair_desc(&new_elem, &heap[0]) < 0) {
                heap_replace_root(heap, k, new_elem);
            }
        }

        Pair* top_k = (Pair*)malloc(k * sizeof(Pair));
        if (!top_k) {
            free(heap);
            return 0;
        }
        memcpy(top_k, heap, k * sizeof(Pair));
        qsort(top_k, k, sizeof(Pair), cmp_pair_desc);

        h = 1469598103934665603ULL;
        for (int i = 0; i < k; ++i) {
            h ^= ((uint64_t)top_k[i].score << 32) ^ top_k[i].index;
            h *= 1099511628211ULL;
        }
        free(top_k);
    }
    free(heap);
    return h;
}