#include "interface.h"
#include <thrust/copy.h>
#include <thrust/sort.h>
#include <cuda_runtime.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0 || iters <= 0) {
        return;
    }

    int* d_temp;
    cudaMalloc(&d_temp, static_cast<size_t>(n) * sizeof(int));
    thrust::copy(input, input + n, d_temp);

    for (int iter = 0; iter < iters; ++iter) {
        thrust::copy(d_temp, d_temp + n, output);
        thrust::sort(output, output + n);
    }

    cudaFree(d_temp);
}