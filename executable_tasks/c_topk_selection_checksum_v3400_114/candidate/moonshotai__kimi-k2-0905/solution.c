#include <stdint.h>
#include <stdlib.h>

static inline void insertion_sort_desc(uint32_t *scores, uint32_t *indices, int l, int r) {
    for (int i = l + 1; i <= r; ++i) {
        uint32_t ts = scores[i];
        uint32_t ti = indices[i];
        int j = i - 1;
        while (j >= l && (scores[j] < ts || (scores[j] == ts && indices[j] > ti))) {
            scores[j + 1] = scores[j];
            indices[j + 1] = indices[j];
            --j;
        }
        scores[j + 1] = ts;
        indices[j + 1] = ti;
    }
}

static inline int partition_desc(uint32_t *scores, uint32_t *indices, int l, int r, uint32_t pivot_score, uint32_t pivot_idx) {
    while (l <= r) {
        while (scores[l] > pivot_score || (scores[l] == pivot_score && indices[l] < pivot_idx)) ++l;
        while (scores[r] < pivot_score || (scores[r] == pivot_score && indices[r] > pivot_idx)) --r;
        if (l <= r) {
            uint32_t tmp_s = scores[l]; scores[l] = scores[r]; scores[r] = tmp_s;
            uint32_t tmp_i = indices[l]; indices[l] = indices[r]; indices[r] = tmp_i;
            ++l; --r;
        }
    }
    return l;
}

static void nth_element_desc(uint32_t *scores, uint32_t *indices, int l, int r, int nth) {
    while (1) {
        int mid = l + ((r - l) >> 1);
        uint32_t pivot_score = scores[mid];
        uint32_t pivot_idx = indices[mid];
        int pos = partition_desc(scores, indices, l, r, pivot_score, pivot_idx);
        if (pos <= nth) {
            if (pos >= nth) break;
            l = pos;
        } else {
            r = pos - 1;
        }
    }
}

uint64_t run(const uint32_t *scores, size_t n, int k, int iters) {
    if (k <= 0 || n == 0) return 0;
    if ((size_t)k > n) k = (int)n;

    uint32_t *buf_scores = (uint32_t *)malloc(n * sizeof(uint32_t) * 2);
    if (!buf_scores) return 0;
    uint32_t *buf_indices = buf_scores + n;

    uint64_t h = 0;
    for (int iter = 0; iter < iters; ++iter) {
        for (size_t i = 0; i < n; ++i) {
            buf_scores[i] = scores[i];
            buf_indices[i] = (uint32_t)i;
        }
        nth_element_desc(buf_scores, buf_indices, 0, (int)n - 1, k - 1);
        insertion_sort_desc(buf_scores, buf_indices, 0, k - 1);
        h = 1469598103934665603ULL;
        for (int i = 0; i < k; ++i) {
            h ^= ((uint64_t)buf_scores[i] << 32) ^ buf_indices[i];
            h *= 1099511628211ULL;
        }
    }
    free(buf_scores);
    return h;
}