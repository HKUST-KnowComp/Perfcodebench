#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    uint32_t score;
    uint32_t index;
} Pair;

static inline int pair_cmp_desc(const Pair a, const Pair b) {
    if (a.score != b.score) return (a.score > b.score) ? -1 : 1;
    return (a.index < b.index) ? -1 : (a.index > b.index);
}

/* in-place partition, returns final pivot position */
static size_t partition(Pair *a, size_t left, size_t right) {
    Pair pivot = a[right];
    size_t i = left;
    for (size_t j = left; j < right; ++j) {
        if (pair_cmp_desc(a[j], pivot) <= 0) {
            Pair tmp = a[i]; a[i] = a[j]; a[j] = tmp;
            ++i;
        }
    }
    Pair tmp = a[i]; a[i] = a[right]; a[right] = tmp;
    return i;
}

/* quickselect: reorder array so that largest k elements are at [0..k-1] */
static void quickselect_topk(Pair *a, size_t n, size_t k) {
    size_t left = 0, right = n - 1;
    while (1) {
        if (left >= right) return;
        size_t p = partition(a, left, right);
        if (p == k - 1) return;
        if (p > k - 1) right = p - 1;
        else left = p + 1;
    }
}

uint64_t run(const uint32_t *scores, size_t n, int k, int iters) {
    static __thread Pair *pairs = NULL;
    static __thread size_t cap = 0;
    if (cap < n) {
        cap = n;
        pairs = (Pair *)realloc(pairs, cap * sizeof(Pair));
        if (!pairs) return 0;
    }

    uint64_t h = 0;
    for (int iter = 0; iter < iters; ++iter) {
        for (size_t i = 0; i < n; ++i) {
            pairs[i].score = scores[i];
            pairs[i].index = (uint32_t)i;
        }
        quickselect_topk(pairs, n, (size_t)k);
        /* sort the top-k slice to ensure deterministic order */
        qsort(pairs, k, sizeof(Pair), (int(*)(const void*,const void*))pair_cmp_desc);

        h = 14695981039346656037ULL;
        for (int i = 0; i < k; ++i) {
            h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
            h *= 1099511628211ULL;
        }
    }
    return h;
}