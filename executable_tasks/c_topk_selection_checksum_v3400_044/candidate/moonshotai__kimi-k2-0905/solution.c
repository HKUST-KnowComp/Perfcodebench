#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    uint32_t score;
    uint32_t index;
} Pair;

static inline int pair_less(const Pair *a, const Pair *b) {
    if (a->score != b->score) return a->score < b->score;
    return a->index > b->index;
}

static inline void swap(Pair *a, Pair *b) {
    Pair tmp = *a;
    *a = *b;
    *b = tmp;
}

static inline size_t median3(Pair *a, size_t i, size_t j, size_t k) {
    return (pair_less(&a[i], &a[j])
            ? (pair_less(&a[j], &a[k]) ? j : (pair_less(&a[i], &a[k]) ? k : i))
            : (pair_less(&a[k], &a[j]) ? j : (pair_less(&a[k], &a[i]) ? k : i)));
}

static void sift_down(Pair *heap, size_t start, size_t end) {
    size_t root = start;
    while (1) {
        size_t child = root * 2 + 1;
        if (child > end) break;
        if (child + 1 <= end && pair_less(&heap[child], &heap[child + 1]))
            child++;
        if (pair_less(&heap[root], &heap[child])) {
            swap(&heap[root], &heap[child]);
            root = child;
        } else break;
    }
}

static void heap_select_topk(Pair *a, size_t n, size_t k) {
    for (int i = (int)(k / 2); i >= 0; --i)
        sift_down(a, (size_t)i, k - 1);
    for (size_t i = k; i < n; ++i) {
        if (pair_less(&a[i], &a[0])) continue;
        swap(&a[0], &a[i]);
        sift_down(a, 0, k - 1);
    }
}

static void introselect_topk(Pair *a, size_t n, size_t k) {
    size_t l = 0, r = n - 1;
    while (1) {
        if (r - l < 32) {
            for (size_t i = l + 1; i <= r; ++i) {
                Pair key = a[i];
                size_t j = i;
                while (j > l && pair_less(&a[j - 1], &key)) {
                    a[j] = a[j - 1];
                    --j;
                }
                a[j] = key;
            }
            return;
        }
        size_t m = median3(a, l, l + (r - l) / 2, r);
        swap(&a[m], &a[r]);
        Pair pivot = a[r];
        size_t i = l, j = r - 1;
        while (1) {
            while (i < r && pair_less(&pivot, &a[i])) ++i;
            while (j > l && pair_less(&a[j], &pivot)) --j;
            if (i >= j) break;
            swap(&a[i], &a[j]);
            ++i; --j;
        }
        swap(&a[i], &a[r]);
        if (k <= i) {
            r = i - 1;
        } else if (k > i + 1) {
            l = i + 1;
        } else {
            break;
        }
    }
}

uint64_t run(const uint32_t *scores, size_t n, int k, int iters) {
    Pair *pairs = (Pair *)malloc(n * sizeof(Pair));
    if (!pairs) return 0;
    uint64_t h = 0;
    for (int iter = 0; iter < iters; ++iter) {
        for (size_t i = 0; i < n; ++i) {
            pairs[i].score = scores[i];
            pairs[i].index = (uint32_t)i;
        }
        if ((size_t)k < n) {
            introselect_topk(pairs, n, k);
            heap_select_topk(pairs, n, k);
        }
        h = 1469598103934665603ULL;
        for (int i = 0; i < k; ++i) {
            h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
            h *= 1099511628211ULL;
        }
    }
    free(pairs);
    return h;
}