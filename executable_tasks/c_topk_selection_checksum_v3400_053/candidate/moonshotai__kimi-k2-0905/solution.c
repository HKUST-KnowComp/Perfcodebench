#include <stdint.h>
#include <stdlib.h>

typedef struct {
    uint32_t score;
    uint32_t index;
} Pair;

/* Hoare partition (descending by score, ascending by index on ties) */
static size_t partition(Pair *a, size_t l, size_t r) {
    Pair pivot = a[l + (r - l) / 2];
    size_t i = l, j = r;
    while (1) {
        while (1) {
            if (a[i].score > pivot.score) break;
            if (a[i].score == pivot.score && a[i].index < pivot.index) break;
            ++i;
        }
        while (1) {
            if (a[j].score < pivot.score) break;
            if (a[j].score == pivot.score && a[j].index > pivot.index) break;
            --j;
        }
        if (i >= j) return j;
        Pair tmp = a[i]; a[i] = a[j]; a[j] = tmp;
        ++i; --j;
    }
}

/* Iterative introselect: places top k elements at the front */
static void nth_element_desc(Pair *a, size_t n, size_t k) {
    size_t l = 0, r = n - 1;
    while (l < r) {
        size_t p = partition(a, l, r);
        if (p < k) {
            l = p + 1;
        } else {
            r = p;
        }
    }
}

uint64_t run(const uint32_t *scores, size_t n, int k, int iters) {
    Pair *pairs = (Pair *)malloc(n * sizeof(Pair));
    if (!pairs) return 0;

    uint64_t h = 0;
    for (int iter = 0; iter < iters; ++iter) {
        /* copy data */
        for (size_t i = 0; i < n; ++i) {
            pairs[i].score = scores[i];
            pairs[i].index = (uint32_t)i;
        }

        /* partial selection of top-k */
        nth_element_desc(pairs, n, k);

        /* insertion sort on the small k prefix for exact order */
        for (size_t i = 1; i < (size_t)k; ++i) {
            Pair key = pairs[i];
            size_t j = i;
            while (j > 0) {
                Pair *prev = &pairs[j - 1];
                if (prev->score > key.score) break;
                if (prev->score == key.score && prev->index < key.index) break;
                pairs[j] = *prev;
                --j;
            }
            pairs[j] = key;
        }

        /* checksum */
        h = 1469598103934665603ULL;
        for (int i = 0; i < k; ++i) {
            h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
            h *= 1099511628211ULL;
        }
    }
    free(pairs);
    return h;
}