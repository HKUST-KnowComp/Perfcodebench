#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    uint32_t score;
    uint32_t index;
} Pair;

/* partition for quickselect: returns final pivot position */
static size_t partition(Pair *a, size_t left, size_t right) {
    Pair pivot = a[right];
    size_t i = left;
    for (size_t j = left; j < right; ++j) {
        /* descending by score, ascending by index on tie */
        if (a[j].score > pivot.score ||
            (a[j].score == pivot.score && a[j].index < pivot.index)) {
            Pair tmp = a[i]; a[i] = a[j]; a[j] = tmp;
            ++i;
        }
    }
    Pair tmp = a[i]; a[i] = a[right]; a[right] = tmp;
    return i;
}

/* select top-k elements into a[0..k-1] (unordered among themselves) */
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

/* simple insertion sort for small k */
static void sort_topk_slice(Pair *a, size_t k) {
    for (size_t i = 1; i < k; ++i) {
        Pair key = a[i];
        size_t j = i;
        while (j > 0) {
            int cmp;
            if (a[j - 1].score > key.score) cmp = 1;
            else if (a[j - 1].score < key.score) cmp = -1;
            else if (a[j - 1].index < key.index) cmp = -1;
            else if (a[j - 1].index > key.index) cmp = 1;
            else cmp = 0;
            if (cmp >= 0) break;
            a[j] = a[j - 1];
            --j;
        }
        a[j] = key;
    }
}

uint64_t run(const uint32_t *scores, size_t n, int k, int iters) {
    if (k <= 0 || (size_t)k > n) k = (int)n;
    Pair *pairs = (Pair *)malloc(n * sizeof(Pair));
    if (!pairs) return 0;

    uint64_t h = 0;
    const uint64_t fnv_offset = 1469598103934665603ULL;
    const uint64_t fnv_prime  = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        /* fill */
        for (size_t i = 0; i < n; ++i) {
            pairs[i].score = scores[i];
            pairs[i].index = (uint32_t)i;
        }
        /* select top-k */
        quickselect_topk(pairs, n, (size_t)k);
        /* sort the top-k slice to establish required order */
        sort_topk_slice(pairs, (size_t)k);
        /* checksum */
        h = fnv_offset;
        for (int i = 0; i < k; ++i) {
            h ^= ((uint64_t)pairs[i].score << 32) | pairs[i].index;
            h *= fnv_prime;
        }
    }
    free(pairs);
    return h;
}