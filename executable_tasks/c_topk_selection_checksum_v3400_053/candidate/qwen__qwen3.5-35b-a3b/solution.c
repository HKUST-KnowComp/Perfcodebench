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

static void heapify_up(Pair* heap, int k, int idx) {
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (cmp_pair_desc(heap[idx], heap[parent]) > 0) {
            Pair temp = heap[idx];
            heap[idx] = heap[parent];
            heap[parent] = temp;
            idx = parent;
        } else {
            break;
        }
    }
}

static void heapify_down(Pair* heap, int k, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < k && cmp_pair_desc(heap[left], heap[smallest]) > 0) {
        smallest = left;
    }

    if (right < k && cmp_pair_desc(heap[right], heap[smallest]) > 0) {
        smallest = right;
    }

    if (smallest != idx) {
        Pair temp = heap[idx];
        heap[idx] = heap[smallest];
        heap[smallest] = temp;

        heapify_down(heap, k, smallest);
    }
}

static void heap_push(Pair* heap, int* heap_size, int k, Pair new_elem) {
    if (*heap_size < k) {
        heap[*heap_size] = new_elem;
        heapify_up(heap, k, *heap_size);
        (*heap_size)++;
    } else {
        if (cmp_pair_desc(new_elem, heap[0]) < 0) {
            heap[0] = new_elem;
            heapify_down(heap, k, 0);
        }
    }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
    if (k <= 0) return 0;
    Pair* heap = (Pair*)malloc(k * sizeof(Pair));
    if (!heap) return 0;
    uint64_t h = 0;
    for (int iter = 0; iter < iters; ++iter) {
        int heap_size = 0;
        for (size_t i = 0; i < n; ++i) {
            Pair p = {scores[i], (uint32_t)i};
            heap_push(heap, &heap_size, k, p);
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