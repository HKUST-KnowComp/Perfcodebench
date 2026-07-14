#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    uint32_t score;
    uint32_t index;
} Pair;

static inline int cmp_pair_desc(const void *lhs, const void *rhs) {
    const Pair *a = (const Pair *)lhs;
    const Pair *b = (const Pair *)rhs;
    if (a->score != b->score) return (a->score < b->score) ? 1 : -1;
    return (a->index < b->index) ? -1 : (a->index > b->index);
}

/* in-place partition, returns final pivot index */
static size_t partition(Pair *a, size_t left, size_t right) {
    Pair pivot = a[right];
    size_t i = left;
    for (size_t j = left; j < right; ++j) {
        if (cmp_pair_desc(&a[j], &pivot) < 0) {
            Pair tmp = a[i]; a[i] = a[j]; a[j] = tmp;
            ++i;
        }
    }
    Pair tmp = a[i]; a[i] = a[right]; a[right] = tmp;
    return i;
}

/* quickselect: ensure smallest k elements (by cmp) are at [0..k-1] */
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
    /* allocate on stack once */
    Pair *pairs = (Pair *)malloc(n * sizeof(Pair));
    if (!pairs) return 0;

    uint64_t h = 0;
    for (int iter = 0; iter < iters; ++iter) {
        /* copy data */
        for (size_t i = 0; i < n; ++i) {
            pairs[i].score = scores[i];
            pairs[i].index = (uint32_t)i;
        }

        /* select top-k */
        quickselect_topk(pairs, n, (size_t)k);
        /* sort only the k winners to obtain exact order */
        qsort(pairs, k, sizeof(Pair), cmp_pair_desc);

        /* hash the ordered top-k */
        h = 14695981039346656037ULL;
        for (int i = 0; i < k; ++i) {
            h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
            h *= 1099511628211ULL;
        }
    }
    free(pairs);
    return h;
}