#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    uint32_t score;
    uint32_t index;
} Pair;

static inline int pair_gt(const Pair a, const Pair b) {
    if (a.score != b.score) return a.score > b.score;
    return a.index < b.index;
}

/* in-place partition, returns final pivot index */
static size_t partition(Pair* a, size_t left, size_t right) {
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
static void topk_quickselect(Pair* a, size_t n, size_t k) {
    size_t left = 0, right = n - 1;
    while (1) {
        if (left >= right) break;
        size_t p = partition(a, left, right);
        if (p == k - 1) break;
        if (p > k - 1) right = p - 1;
        else left = p + 1;
    }
}

/* sort only the first k elements descending */
static void sort_topk(Pair* a, size_t k) {
    for (size_t i = 1; i < k; ++i) {
        Pair key = a[i];
        size_t j = i;
        while (j > 0 && pair_gt(key, a[j - 1])) {
            a[j] = a[j - 1];
            --j;
        }
        a[j] = key;
    }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
    static Pair* pairs = NULL;
    static size_t cap = 0;
    if (n > cap) {
        pairs = (Pair*)realloc(pairs, n * sizeof(Pair));
        cap = n;
    }
    if (!pairs) return 0;

    uint64_t h = 0;
    for (int iter = 0; iter < iters; ++iter) {
        for (size_t i = 0; i < n; ++i) {
            pairs[i].score = scores[i];
            pairs[i].index = (uint32_t)i;
        }
        topk_quickselect(pairs, n, (size_t)k);
        sort_topk(pairs, (size_t)k);
        h = 14695981039346656037ULL;
        for (int i = 0; i < k; ++i) {
            h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
            h *= 1099511628211ULL;
        }
    }
    return h;
}