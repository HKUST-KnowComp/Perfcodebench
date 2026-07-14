#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_ptr.h>
#include <thrust/sort.h>
#include <thrust/copy.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    for (int iter = 0; iter < iters; ++iter) {
        thrust::copy(input, input + n, output);
        thrust::sort(thrust::device_ptr<int>(output), thrust::device_ptr<int>(output + n));
        cudaDeviceSynchronize();
    }
}