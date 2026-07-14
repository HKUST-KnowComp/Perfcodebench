#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static int cmp_pair_desc(const Pair* lhs, const Pair* rhs) {
  const Pair* a = (const Pair*) lhs;
  const Pair* b = (const Pair*) rhs;
  if (a->score < b->score) return 1;
  if (a->score > b->score) return -1;
  if (a->index < b->index) return -1;
  if (a->index > b->index) return 1;
  return 0;
}

static int heap_cmp(const Pair* a, const Pair* b) {
    if (a->score < b->score) return -1;
    if (a->score > b->score) return 1;
    if (a->index > b->index) return -1;
    if (a->index < b->index) return 1;
    return 0;
}

static void sift_up(Pair* heap, int idx, int size) {
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (heap_cmp(&heap[idx], &heap[parent]) < 0) {
            Pair temp = heap[idx];
            heap[idx] = heap[parent];
            heap[parent] = temp;
            idx = parent;
        } else {
            break;
        }
    }
}

static void sift_down(Pair* heap, int idx, int size) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < size && heap_cmp(&heap[left], &heap[smallest]) < 0) {
        smallest = left;
    }
    if (right < size && heap_cmp(&heap[right], &heap[smallest]) < 0) {
        smallest = right;
    }
    if (smallest != idx) {
        Pair temp = heap[idx];
        heap[idx] = heap[smallest];
        heap[smallest] = temp;
        sift_down(heap, smallest, size);
    }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
    if (k <= 0 || n == 0) return 0;
    if (k > n) k = n;

    Pair* heap = (Pair*)malloc(k * sizeof(Pair));
    if (!heap) return 0;

    Pair* top_k = (Pair*)malloc(k * sizeof(Pair));
    if (!top_k) {
        free(heap);
        return 0;
    }

    uint64_t h = 0;
    for (int iter = 0; iter < iters; ++iter) {
        int heap_size = 0;
        for (size_t i = 0; i < n; ++i) {
            Pair p = {scores[i], (uint32_t)i};
            if (heap_size < k) {
                heap[heap_size++] = p;
                sift_up(heap, heap_size - 1, k);
            } else {
                if (heap_cmp(&p, &heap[0]) > 0) {
                    heap[0] = p;
                    sift_down(heap, 0, k);
                }
            }
        }

        memcpy(top_k, heap, k * sizeof(Pair));
        qsort(top_k, k, sizeof(Pair), cmp_pair_desc);

        h = 1469598103934665603ULL;
        for (int i = 0; i < k; ++i) {
            h ^= ((uint64_t)top_k[i].score << 32) ^ top_k[i].index;
            h *= 1099511628211ULL;
        }
    }

    free(heap);
    free(top_k);
    return h;
}