#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <thrust/sort.h>
#include <thrust/copy.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0 || iters <= 0) {
        return;
    }

    const thrust::host_vector<int> h_input(input, input + n);
    thrust::device_vector<int> d_original(n);
    d_original = h_input;
    thrust::device_vector<int> d_sorted(n);

    for (int iter = 0; iter < iters; ++iter) {
        d_sorted = d_original;
        thrust::sort(d_sorted.begin(), d_sorted.end());
        thrust::copy(d_sorted.begin(), d_sorted.end(), output);
    }
}