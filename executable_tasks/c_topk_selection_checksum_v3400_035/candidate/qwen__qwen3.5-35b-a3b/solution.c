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

static void sift_up(Pair* heap, int idx) {
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (cmp_pair_desc(&heap[idx], &heap[parent]) > 0) {
            Pair temp = heap[idx];
            heap[idx] = heap[parent];
            heap[parent] = temp;
            idx = parent;
        } else {
            break;
        }
    }
}

static void sift_down(Pair* heap, int idx, int k) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < k && cmp_pair_desc(&heap[left], &heap[smallest]) > 0) {
        smallest = left;
    }
    if (right < k && cmp_pair_desc(&heap[right], &heap[smallest]) > 0) {
        smallest = right;
    }

    if (smallest != idx) {
        Pair temp = heap[idx];
        heap[idx] = heap[smallest];
        heap[smallest] = temp;
        sift_down(heap, smallest, k);
    }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
    if (k <= 0 || k > n) {
        return 0;
    }

    Pair* heap = (Pair*)malloc(k * sizeof(Pair));
    if (!heap) return 0;

    Pair* top_k = (Pair*)malloc(k * sizeof(Pair));
    if (!top_k) {
        free(heap);
        return 0;
    }

    uint64_t h = 0;
    for (int iter = 0; iter < iters; ++iter) {
        for (int i = 0; i < k; ++i) {
            heap[i].score = scores[i];
            heap[i].index = i;
            sift_up(heap, i);
        }

        for (size_t i = k; i < n; ++i) {
            Pair current = {scores[i], (uint32_t)i};
            if (cmp_pair_desc(&current, &heap[0]) < 0) {
                heap[0] = current;
                sift_down(heap, 0, k);
            }
        }

        for (int i = 0; i < k; ++i) {
            top_k[i] = heap[i];
        }
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