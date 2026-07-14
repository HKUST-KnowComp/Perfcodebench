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

static inline void sift_down(Pair* heap, int start, int end) {
    int root = start;
    while (1) {
        int child = 2 * root + 1;
        if (child > end) break;
        int swap = root;
        if (heap[child].score < heap[swap].score ||
            (heap[child].score == heap[swap].score && heap[child].index > heap[swap].index)) {
            swap = child;
        }
        if (child + 1 <= end) {
            if (heap[child+1].score < heap[swap].score ||
                (heap[child+1].score == heap[swap].score && heap[child+1].index > heap[swap].index)) {
                swap = child + 1;
            }
        }
        if (swap == root) break;
        Pair tmp = heap[root];
        heap[root] = heap[swap];
        heap[swap] = tmp;
        root = swap;
    }
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
        for (int i = k / 2 - 1; i >= 0; --i) {
            sift_down(heap, i, k - 1);
        }

        for (size_t i = k; i < n; ++i) {
            uint32_t s = scores[i];
            uint32_t rs = heap[0].score;
            if (s < rs) continue;
            if (s == rs && (uint32_t)i > heap[0].index) continue;

            heap[0].score = s;
            heap[0].index = (uint32_t)i;
            sift_down(heap, 0, k - 1);
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