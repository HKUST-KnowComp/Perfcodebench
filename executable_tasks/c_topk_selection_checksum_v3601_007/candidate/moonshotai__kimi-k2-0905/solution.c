#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    uint32_t score;
    uint32_t index;
} Pair;

static inline void swap_pair(Pair *a, Pair *b) {
    Pair t = *a; *a = *b; *b = t;
}

/* partition for quickselect: returns final pivot position */
static size_t partition(Pair *a, size_t left, size_t right) {
    Pair pivot = a[right];
    size_t i = left;
    for (size_t j = left; j < right; ++j) {
        int c = (a[j].score > pivot.score) -
                (a[j].score < pivot.score);
        if (c == 0) c = (a[j].index < pivot.index) -
                        (a[j].index > pivot.index);
        if (c < 0) {
            swap_pair(&a[i], &a[j]);
            ++i;
        }
    }
    swap_pair(&a[i], &a[right]);
    return i;
}

/* quickselect: places the k-th smallest element in its sorted position */
static void quickselect(Pair *a, size_t n, size_t k) {
    size_t left = 0, right = n - 1;
    while (1) {
        if (left == right) return;
        size_t p = partition(a, left, right);
        if (p == k) return;
        else if (k < p) right = p - 1;
        else left = p + 1;
    }
}

/* insertion sort for small slices */
static void insertion_sort(Pair *a, size_t n) {
    for (size_t i = 1; i < n; ++i) {
        Pair key = a[i];
        size_t j = i;
        while (j > 0) {
            int c = (a[j-1].score > key.score) -
                    (a[j-1].score < key.score);
            if (c == 0) c = (a[j-1].index < key.index) -
                            (a[j-1].index > key.index);
            if (c <= 0) break;
            a[j] = a[j-1];
            --j;
        }
        a[j] = key;
    }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
    if (k <= 0 || n == 0) return 0;
    if ((size_t)k > n) k = (int)n;
    Pair *pairs = (Pair*)malloc(n * sizeof(Pair));
    if (!pairs) return 0;
    uint64_t h = 0;
    for (int iter = 0; iter < iters; ++iter) {
        /* fill buffer */
        for (size_t i = 0; i < n; ++i) {
            pairs[i].score = scores[i];
            pairs[i].index = (uint32_t)i;
        }
        if ((size_t)k == n) {
            /* already full sort needed */
            insertion_sort(pairs, n);
        } else {
            /* quickselect to isolate top-k */
            quickselect(pairs, n, k);
            /* sort the top-k slice to ensure order */
            insertion_sort(pairs, k);
        }
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