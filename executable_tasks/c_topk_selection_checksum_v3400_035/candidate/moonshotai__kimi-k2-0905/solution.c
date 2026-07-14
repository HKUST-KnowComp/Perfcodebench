#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    uint32_t score;
    uint32_t index;
} Pair;

static inline int pair_gt(const Pair a, const Pair b) {
    return (a.score > b.score) || (a.score == b.score && a.index < b.index);
}

/* in-place partition, returns final pivot position */
static size_t partition(Pair *a, size_t left, size_t right) {
    Pair pivot = a[right];
    size_t i = left;
    for (size_t j = left; j < right; ++j) {
        if (pair_gt(a[j], pivot)) {
            Pair tmp = a[i]; a[i] = a[j]; a[j] = tmp;
            ++i;
        }
    }
    Pair tmp = a[i]; a[i] = a[right]; a[right] = tmp;
    return i;
}

/* quickselect: ensure the k largest elements are at the front [0..k-1] */
static void quickselect_topk(Pair *a, size_t n, size_t k) {
    size_t left = 0, right = n - 1;
    while (1) {
        if (left >= right) return;
        size_t p = partition(a, left, right);
        if (p == k) return;
        if (p < k) left = p + 1;
        else right = p - 1;
    }
}

/* small-array quicksort for the final k items */
static void small_qsort(Pair *a, size_t left, size_t right) {
    while (left < right) {
        size_t p = partition(a, left, right);
        /* tail-recursion elimination: sort smaller side first */
        if (p - left < right - p) {
            small_qsort(a, left, p - 1);
            left = p + 1;
        } else {
            small_qsort(a, p + 1, right);
            right = p - 1;
        }
    }
}

uint64_t run(const uint32_t *scores, size_t n, int k, int iters) {
    Pair *pairs = (Pair *)malloc(n * sizeof(Pair));
    if (!pairs) return 0;

    uint64_t h = 0;
    for (int iter = 0; iter < iters; ++iter) {
        /* fill array once per iteration */
        for (size_t i = 0; i < n; ++i) {
            pairs[i].score = scores[i];
            pairs[i].index = (uint32_t)i;
        }

        /* select top-k */
        quickselect_topk(pairs, n, (size_t)k);
        /* sort those k to establish required order */
        small_qsort(pairs, 0, k - 1);

        /* compute checksum */
        h = 1469598103934665603ULL;
        for (int i = 0; i < k; ++i) {
            h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
            h *= 1099511628211ULL;
        }
    }
    free(pairs);
    return h;
}