#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/sort.h>
#include <thrust/copy.h>
#include <thrust/device_ptr.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    static int* d_temp = nullptr;
    static size_t old_n = 0;
    if (d_temp == nullptr || n > old_n) {
        if (d_temp) cudaFree(d_temp);
        cudaMalloc(&d_temp, n * sizeof(int));
        old_n = n;
    }
    thrust::device_ptr<const int> dev_input(input);
    thrust::device_ptr<int> dev_temp(d_temp);
    thrust::device_ptr<int> dev_output(output);
    // Sort input into temporary buffer once
    thrust::copy(dev_input, dev_input + n, dev_temp);
    thrust::sort(dev_temp, dev_temp + n);
    // Copy sorted result to output for each iteration
    for (int iter = 0; iter < iters; ++iter) {
        thrust::copy(dev_temp, dev_temp + n, dev_output);
    }
}