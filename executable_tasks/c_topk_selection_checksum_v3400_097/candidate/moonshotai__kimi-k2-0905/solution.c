#include <stdint.h>
#include <stdlib.h>

typedef struct {
    uint32_t score;
    uint32_t index;
} Pair;

static inline int pair_cmp_desc(const Pair a, const Pair b) {
    return (a.score > b.score) ? -1 :
           (a.score < b.score) ?  1 :
           (a.index < b.index) ? -1 :
           (a.index > b.index) ?  1 : 0;
}

static inline void swap_pair(Pair *a, Pair *b) {
    Pair tmp = *a;
    *a = *b;
    *b = tmp;
}

static size_t partition(Pair *a, size_t left, size_t right) {
    Pair pivot = a[right];
    size_t i = left;
    for (size_t j = left; j < right; ++j) {
        if (pair_cmp_desc(a[j], pivot) < 0) {
            swap_pair(&a[i], &a[j]);
            ++i;
        }
    }
    swap_pair(&a[i], &a[right]);
    return i;
}

static void insertion_sort_desc(Pair *a, size_t n) {
    for (size_t i = 1; i < n; ++i) {
        Pair key = a[i];
        size_t j = i;
        while (j > 0 && pair_cmp_desc(a[j-1], key) > 0) {
            a[j] = a[j-1];
            --j;
        }
        a[j] = key;
    }
}

static void nth_element_desc(Pair *a, size_t n, size_t k) {
    size_t lo = 0, hi = n - 1;
    while (lo < hi) {
        size_t p = partition(a, lo, hi);
        if (p == k)      return;
        else if (p < k)  lo = p + 1;
        else             hi = p - 1;
    }
}

uint64_t run(const uint32_t *scores, size_t n, int k, int iters) {
    Pair *pairs = (Pair*)malloc(n * sizeof(Pair));
    if (!pairs) return 0;

    uint64_t h = 0;
    for (int iter = 0; iter < iters; ++iter) {
        for (size_t i = 0; i < n; ++i) {
            pairs[i].score = scores[i];
            pairs[i].index = (uint32_t)i;
        }

        if ((size_t)k < n) {
            nth_element_desc(pairs, n, (size_t)k);
        }
        insertion_sort_desc(pairs, (size_t)k);

        h = 1469598103934665603ULL;
        for (int i = 0; i < k; ++i) {
            h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
            h *= 1099511628211ULL;
        }
    }
    free(pairs);
    return h;
}