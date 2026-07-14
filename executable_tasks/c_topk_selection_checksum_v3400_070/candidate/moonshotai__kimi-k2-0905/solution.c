#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    uint32_t score;
    uint32_t index;
} Pair;

static inline int cmp_pair_desc(const Pair a, const Pair b) {
    if (a.score != b.score) return (a.score > b.score) ? -1 : 1;
    return (a.index < b.index) ? -1 : (a.index > b.index);
}

/* median-of-three for quickselect pivot */
static inline Pair median3(Pair a, Pair b, Pair c) {
    int ab = cmp_pair_desc(a, b);
    int bc = cmp_pair_desc(b, c);
    int ac = cmp_pair_desc(a, c);
    if (ab <= 0) {
        if (bc <= 0) return b;
        return (ac <= 0) ? a : c;
    } else {
        if (ac <= 0) return a;
        return (bc <= 0) ? c : b;
    }
}

/* partition around pivot; returns final pivot index */
static size_t partition(Pair* a, size_t l, size_t r, Pair pivot) {
    while (1) {
        while (cmp_pair_desc(a[l], pivot) < 0) ++l;
        while (cmp_pair_desc(a[r], pivot) > 0) --r;
        if (l >= r) return r;
        Pair tmp = a[l]; a[l] = a[r]; a[r] = tmp;
        ++l; --r;
    }
}

/* quickselect: ensure first k elements are the top-k (unordered among themselves) */
static void topk_quickselect(Pair* a, size_t n, size_t k) {
    size_t l = 0, r = n - 1;
    while (1) {
        if (l >= r) break;
        size_t m = l + ((r - l) >> 1);
        Pair pivot = median3(a[l], a[m], a[r]);
        size_t p = partition(a, l, r, pivot);
        size_t left_size = p - l + 1;
        if (k <= left_size) {
            r = p;
        } else {
            k -= left_size;
            l = p + 1;
        }
    }
}

/* insertion sort for small n */
static void insertion_desc(Pair* a, size_t n) {
    for (size_t i = 1; i < n; ++i) {
        Pair key = a[i];
        size_t j = i;
        while (j > 0 && cmp_pair_desc(a[j - 1], key) > 0) {
            a[j] = a[j - 1];
            --j;
        }
        a[j] = key;
    }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
    if (k <= 0 || n == 0) return 0;
    if ((size_t)k > n) k = (int)n;

    Pair* pairs = (Pair*)malloc(n * sizeof(Pair));
    if (!pairs) return 0;

    uint64_t h = 0;
    for (int iter = 0; iter < iters; ++iter) {
        /* fill array */
        for (size_t i = 0; i < n; ++i) {
            pairs[i].score = scores[i];
            pairs[i].index = (uint32_t)i;
        }

        /* select top-k */
        topk_quickselect(pairs, n, (size_t)k);

        /* sort only the top-k slice to establish exact order */
        if ((size_t)k <= 16) {
            insertion_desc(pairs, (size_t)k);
        } else {
            qsort(pairs, (size_t)k, sizeof(Pair),
                  (int(*)(const void*,const void*))cmp_pair_desc);
        }

        /* compute checksum with 4-way unroll */
        h = 14695981039346656037ULL;
        int i = 0;
        for (; i + 3 < k; i += 4) {
            h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
            h *= 1099511628211ULL;
            h ^= ((uint64_t)pairs[i+1].score << 32) ^ pairs[i+1].index;
            h *= 1099511628211ULL;
            h ^= ((uint64_t)pairs[i+2].score << 32) ^ pairs[i+2].index;
            h *= 1099511628211ULL;
            h ^= ((uint64_t)pairs[i+3].score << 32) ^ pairs[i+3].index;
            h *= 1099511628211ULL;
        }
        for (; i < k; ++i) {
            h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
            h *= 1099511628211ULL;
        }
    }
    free(pairs);
    return h;
}