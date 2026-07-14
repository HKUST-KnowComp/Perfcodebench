#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_ptr.h>
#include <thrust/sort.h>
#include <thrust/execution_policy.h>

namespace {
__global__ void copy_kernel(const int* __restrict__ src,
                            int* __restrict__ dst, int n) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < n) dst[idx] = src[idx];
}
} // namespace

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    for (int iter = 0; iter < iters; ++iter) {
        if (input != output) {
            const int block = 256;
            const int grid  = (n + block - 1) / block;
            copy_kernel<<<grid, block>>>(input, output, n);
            cudaDeviceSynchronize();
        }

        thrust::device_ptr<int> dev_out(const_cast<int*>(output));
        thrust::sort(thrust::device, dev_out, dev_out + n);
    }
    cudaDeviceSynchronize();
}