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

static inline void sift_down(Pair* heap, int size, int idx) {
    Pair val = heap[idx];
    int child;
    while ((child = 2 * idx + 1) < size) {
        int right = child + 1;
        if (right < size) {
            if (heap[right].score < heap[child].score ||
                (heap[right].score == heap[child].score && heap[right].index > heap[child].index)) {
                child = right;
            }
        }
        if (val.score < heap[child].score ||
            (val.score == heap[child].score && val.index > heap[child].index)) {
            break;
        }
        heap[idx] = heap[child];
        idx = child;
    }
    heap[idx] = val;
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
    if (k <= 0) return 1469598103934665603ULL;
    if ((size_t)k > n) k = (int)n;

    Pair* heap = (Pair*)malloc(k * sizeof(Pair));
    if (!heap) return 0;

    uint64_t h = 0;
    for (int iter = 0; iter < iters; ++iter) {
        for (int i = 0; i < k; ++i) {
            heap[i].score = scores[i];
            heap[i].index = (uint32_t)i;
        }
        for (int i = k / 2 - 1; i >= 0; --i) sift_down(heap, k, i);

        for (size_t i = k; i < n; ++i) {
            uint32_t s = scores[i];
            if (s > heap[0].score || (s == heap[0].score && (uint32_t)i < heap[0].index)) {
                heap[0].score = s;
                heap[0].index = (uint32_t)i;
                sift_down(heap, k, 0);
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