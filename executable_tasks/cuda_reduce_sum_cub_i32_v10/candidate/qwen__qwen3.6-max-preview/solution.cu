#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    int sum = 0;
    int stride = blockDim.x * gridDim.x;
    for (int i = blockIdx.x * blockDim.x + threadIdx.x; i < n; i += stride) {
        sum += input[i];
    }

    __shared__ int sdata[32];
    int lane = threadIdx.x % 32;
    int wid = threadIdx.x / 32;

    // Warp-level reduction
    #pragma unroll
    for (int offset = 16; offset > 0; offset /= 2) {
        sum += __shfl_down_sync(0xffffffff, sum, offset);
    }

    // Write warp results to shared memory
    if (lane == 0) sdata[wid] = sum;
    __syncthreads();

    // Final reduction within first warp
    if (wid == 0) {
        int warp_sum = (lane < blockDim.x / 32) ? sdata[lane] : 0;
        #pragma unroll
        for (int offset = 16; offset > 0; offset /= 2) {
            warp_sum += __shfl_down_sync(0xffffffff, warp_sum, offset);
        }
        if (lane == 0) {
            atomicAdd(output, warp_sum);
        }
    }
}

} // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    constexpr int kBlock = 256;
    int grid = (n + kBlock - 1) / kBlock;
    if (grid > 1024) grid = 1024;
    if (grid < 1) grid = 1;
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<<<grid, kBlock>>>(input, output, n);
    }
}