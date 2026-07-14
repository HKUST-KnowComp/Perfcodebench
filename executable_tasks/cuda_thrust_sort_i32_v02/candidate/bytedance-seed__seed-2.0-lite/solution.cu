#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/sort.h>
#include <thrust/device_ptr.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    for (int iter = 0; iter < iters; ++iter) {
        // Reset output from original input to match baseline's per-iteration sort setup
        cudaMemcpy(output, input, n * sizeof(int), cudaMemcpyDeviceToDevice);
        // Wrap device pointer for Thrust's GPU-accelerated algorithms
        thrust::device_ptr<int> d_out(output);
        // Parallel ascending sort, correct order matching baseline insertion sort
        thrust::sort(d_out, d_out + n);
    }
}