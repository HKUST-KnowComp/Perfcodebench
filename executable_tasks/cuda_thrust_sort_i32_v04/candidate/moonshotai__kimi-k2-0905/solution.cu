#include "interface.h"
#include <thrust/device_ptr.h>
#include <thrust/sort.h>
#include <thrust/execution_policy.h>
#include <cuda_runtime.h>

static int* d_tmp = nullptr;
static size_t tmp_bytes = 0;

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    // Allocate temporary buffer once and reuse
    if (d_tmp == nullptr) {
        cudaMalloc(&d_tmp, sizeof(int) * n);
        tmp_bytes = sizeof(int) * n;
    }
    else if (tmp_bytes < sizeof(int) * n) {
        cudaFree(d_tmp);
        cudaMalloc(&d_tmp, sizeof(int) * n);
        tmp_bytes = sizeof(int) * n;
    }

    // Copy input to temporary buffer
    cudaMemcpy(d_tmp, input, sizeof(int) * n, cudaMemcpyDeviceToDevice);

    // Wrap raw pointer with thrust device_ptr
    thrust::device_ptr<int> ptr(d_tmp);

    for (int iter = 0; iter < iters; ++iter) {
        // Sort in-place
        thrust::sort(thrust::device, ptr, ptr + n);
    }

    // Copy sorted result to output
    cudaMemcpy(output, d_tmp, sizeof(int) * n, cudaMemcpyDeviceToDevice);
}