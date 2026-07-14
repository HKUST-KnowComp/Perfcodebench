#include "interface.h"

#include <cstdint>
#include <vector>
#include <algorithm>
#include <omp.h>

static void parallel_merge(int32_t* arr, int32_t* tmp, int64_t left, int64_t mid, int64_t right) {
    int64_t i = left, j = mid, k = left;
    while (i < mid && j < right) {
        if (arr[i] <= arr[j]) {
            tmp[k++] = arr[i++];
        } else {
            tmp[k++] = arr[j++];
        }
    }
    while (i < mid) tmp[k++] = arr[i++];
    while (j < right) tmp[k++] = arr[j++];
    for (int64_t x = left; x < right; ++x) arr[x] = tmp[x];
}

static void parallel_merge_sort(int32_t* arr, int32_t* tmp, int64_t left, int64_t right, int depth) {
    if (right - left <= 1) return;
    if (right - left < 10000 || depth > 4) {
        std::sort(arr + left, arr + right);
        return;
    }
    int64_t mid = left + (right - left) / 2;
    #pragma omp task shared(arr, tmp) if(depth < 4)
    parallel_merge_sort(arr, tmp, left, mid, depth + 1);
    #pragma omp task shared(arr, tmp) if(depth < 4)
    parallel_merge_sort(arr, tmp, mid, right, depth + 1);
    #pragma omp taskwait
    parallel_merge(arr, tmp, left, mid, right);
}

static int64_t compute_sum_sorted(const int32_t* sorted, int64_t n) {
    int64_t total = 0;
    #pragma omp parallel for reduction(+:total) schedule(static)
    for (int64_t i = 0; i < n; ++i) {
        int64_t coeff = 2 * i - n + 1;
        total += static_cast<int64_t>(sorted[i]) * coeff;
    }
    return total;
}

int64_t allpairs_l1_sum(const std::vector<int32_t>& xs, const std::vector<int32_t>& ys, int iters) {
    int64_t total = 0;
    const int64_t n = static_cast<int64_t>(xs.size());
    if (n <= 1) return 0;
    
    std::vector<int32_t> sx(n), sy(n), tmp(n);
    
    for (int iter = 0; iter < iters; ++iter) {
        for (int64_t i = 0; i < n; ++i) {
            sx[i] = xs[i];
            sy[i] = ys[i];
        }
        
        #pragma omp parallel
        {
            #pragma omp single
            {
                parallel_merge_sort(sx.data(), tmp.data(), 0, n, 0);
            }
        }
        
        int64_t sum_x = compute_sum_sorted(sx.data(), n);
        
        #pragma omp parallel
        {
            #pragma omp single
            {
                parallel_merge_sort(sy.data(), tmp.data(), 0, n, 0);
            }
        }
        
        int64_t sum_y = compute_sum_sorted(sy.data(), n);
        
        total = sum_x + sum_y;
    }
    return total;
}