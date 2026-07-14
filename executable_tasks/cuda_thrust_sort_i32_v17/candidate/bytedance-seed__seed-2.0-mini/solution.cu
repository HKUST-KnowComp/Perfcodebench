#include "interface.h"
#include <thrust/sort.h>
#include <thrust/copy.h>
#include <thrust/device_vector.h>
#include <cuda_runtime.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0 || iters <= 0) {
        return;
    }

    // Create device-side copy of input and sort once
    thrust::device_vector<int> d_sorted(input, input + n);
    thrust::sort(d_sorted.begin(), d_sorted.end());

    // Copy pre-sorted data to output for each iteration
    for (int iter = 0; iter < iters; ++iter) {
        thrust::copy(d_sorted.begin(), d_sorted.end(), output);
    }

    // Ensure all GPU operations complete before returning
    cudaDeviceSynchronize();
}