#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    uint32_t score;
    uint32_t index;
} Pair;

static inline void swap_pair(Pair* a, Pair* b) {
    Pair tmp = *a;
    *a = *b;
    *b = tmp;
}

/* median-of-three pivot selection */
static inline Pair* median_of_three(Pair* a, Pair* b, Pair* c) {
    if (cmp_pair_desc(a, b) < 0) swap_pair(a, b);
    if (cmp_pair_desc(a, c) < 0) swap_pair(a, c);
    if (cmp_pair_desc(b, c) < 0) swap_pair(b, c);
    return b;
}

static inline int cmp_pair_desc(const void* lhs, const void* rhs) {
    const Pair* a = (const Pair*)lhs;
    const Pair* b = (const Pair*)rhs;
    if (a->score != b->score) return (a->score < b->score) ? 1 : -1;
    return (a->index < b->index) ? -1 : (a->index > b->index);
}

/* partition for quickselect: returns index of pivot after partition */
static size_t partition(Pair* arr, size_t left, size_t right) {
    Pair* pivot = median_of_three(&arr[left], &arr[left + (right - left) / 2], &arr[right]);
    uint32_t pivot_score = pivot->score;
    uint32_t pivot_index = pivot->index;

    while (left <= right) {
        while (cmp_pair_desc(&arr[left], pivot) < 0) ++left;
        while (cmp_pair_desc(&arr[right], pivot) > 0) --right;
        if (left <= right) {
            swap_pair(&arr[left], &arr[right]);
            ++left;
            --right;
        }
    }
    return left;
}

/* iterative quickselect: largest k elements reside at the front afterwards */
static void quickselect_topk(Pair* arr, size_t n, size_t k) {
    size_t left = 0, right = n - 1;
    while (1) {
        if (left >= right) break;
        size_t mid = partition(arr, left, right);
        if (mid >= k)
            right = mid - 1;
        else
            left = mid;
    }
}

static int cmp_pair_desc_small(const void* lhs, const void* rhs) {
    return cmp_pair_desc(lhs, rhs);
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
    if (k <= 0 || n == 0) return 0;
    if ((size_t)k > n) k = (int)n;

    /* Use stack buffer for small k to avoid malloc/free per iteration */
    Pair stack_buf[64];
    Pair* topk = (k <= 64) ? stack_buf : (Pair*)malloc(k * sizeof(Pair));
    if (!topk) return 0;

    uint64_t h = 0;
    for (int iter = 0; iter < iters; ++iter) {
        /* On-stack buffer for entire dataset, avoiding malloc/free per iter */
        Pair* pairs = (n <= 65536) ? (Pair*)malloc(n * sizeof(Pair)) : (Pair*)malloc(n * sizeof(Pair));
        if (!pairs) {
            if (topk != stack_buf) free(topk);
            return 0;
        }

        /* Fill pairs */
        for (size_t i = 0; i < n; ++i) {
            pairs[i].score = scores[i];
            pairs[i].index = (uint32_t)i;
        }

        /* Quickselect top-k */
        quickselect_topk(pairs, n, (size_t)k);

        /* Copy out the first k elements */
        memcpy(topk, pairs, k * sizeof(Pair));
        free(pairs);

        /* Sort the k elements to establish final order */
        qsort(topk, k, sizeof(Pair), cmp_pair_desc_small);

        /* Compute checksum */
        h = 1469598103934665603ULL;
        for (int i = 0; i < k; ++i) {
            h ^= ((uint64_t)topk[i].score << 32) | topk[i].index;
            h *= 1099511628211ULL;
        }
    }

    if (topk != stack_buf) free(topk);
    return h;
}