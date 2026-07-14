#include "interface.h"
#include <thrust/device_vector.h>
#include <thrust/sort.h>
#include <thrust/copy.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0 || iters <= 0) {
        return;
    }
    thrust::device_vector<int> d_original(input, input + n);
    thrust::device_vector<int> d_work(n);

    for (int iter = 0; iter < iters; ++iter) {
        thrust::copy(d_original.begin(), d_original.end(), d_work.begin());
        thrust::sort(d_work.begin(), d_work.end());
    }

    thrust::copy(d_work.begin(), d_work.end(), output);
}