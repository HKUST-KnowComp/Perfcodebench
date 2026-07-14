#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    uint32_t score;
    uint32_t index;
} Pair;

static inline int cmp_pair_desc_indexed(const void *lhs, const void *rhs) {
    const Pair *a = (const Pair *)lhs;
    const Pair *b = (const Pair *)rhs;
    if (a->score < b->score) return 1;
    if (a->score > b->score) return -1;
    if (a->index < b->index) return -1;
    if (a->index > b->index) return 1;
    return 0;
}

/* Lomuto-style partitioning: returns final pivot index */
static size_t partition(Pair *arr, size_t left, size_t right, size_t pivot_idx) {
    Pair pivot = arr[pivot_idx];
    Pair tmp;
    tmp = arr[pivot_idx];
    arr[pivot_idx] = arr[right];
    arr[right] = tmp;

    size_t i = left;
    for (size_t j = left; j < right; ++j) {
        /* score dominates; indices break ties; want bigger at left */
        if (arr[j].score > pivot.score ||
            (arr[j].score == pivot.score && arr[j].index < pivot.index)) {
            tmp = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp;
            i++;
        }
    }
    tmp = arr[i];
    arr[i] = arr[right];
    arr[right] = tmp;
    return i;
}

/* Iterative quickselect: brings the k-th largest into arr[k], every item >= k left of k */
static void quickselect(Pair *arr, size_t l, size_t r, size_t k) {
    while (l < r) {
        size_t pivot = l + ((r - l) >> 1);
        size_t p = partition(arr, l, r, pivot);
        if (p == k) return;
        if (p > k) r = p - 1;
        else l = p + 1;
    }
}

uint64_t run(const uint32_t *scores, size_t n, int k, int iters) {
    Pair *pairs = (Pair *)malloc(n * sizeof(Pair));
    if (!pairs) return 0;

    uint64_t h = 0;

    for (int iter = 0; iter < iters; ++iter) {
        /* build the data */
        for (size_t i = 0; i < n; ++i) {
            pairs[i].score = scores[i];
            pairs[i].index = (uint32_t)i;
        }

        /* select the k best into first k positions */
        quickselect(pairs, 0, n - 1, k - 1);
        /* sort those first k (k is tiny) */
        qsort(pairs, k, sizeof(Pair), cmp_pair_desc_indexed);

        /* hash in order */
        h = 14695981039346656037ULL;
        for (int i = 0; i < k; ++i) {
            h ^= ((uint64_t)pairs[i].score << 32) | pairs[i].index;
            h *= 1099511628211ULL;
        }
    }
    free(pairs);
    return h;
}