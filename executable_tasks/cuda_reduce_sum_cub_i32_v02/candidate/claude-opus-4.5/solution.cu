#include "interface.h"
#include <cuda_runtime.h>

namespace {

__device__ __forceinline__ int warp_reduce_sum(int val) {
    #pragma unroll
    for (int offset = 16; offset > 0; offset >>= 1) {
        val += __shfl_down_sync(0xffffffff, val, offset);
    }
    return val;
}

__global__ void reduce_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    int sum = 0;
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    // Vectorized loads: process 4 ints at a time
    int n4 = n / 4;
    const int4* input4 = reinterpret_cast<const int4*>(input);
    for (int i = tid; i < n4; i += stride) {
        int4 v = input4[i];
        sum += v.x + v.y + v.z + v.w;
    }

    // Handle remaining elements
    int base = n4 * 4;
    for (int i = base + tid; i < n; i += stride) {
        sum += input[i];
    }

    // Warp-level reduction
    sum = warp_reduce_sum(sum);

    // First thread of each warp does atomic add
    if ((threadIdx.x & 31) == 0) {
        atomicAdd(output, sum);
    }
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    constexpr int kBlock = 256;
    int grid = (n + kBlock * 4 - 1) / (kBlock * 4);
    if (grid > 1024) grid = 1024;
    if (grid < 1) grid = 1;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, sizeof(int));
        reduce_kernel<<<grid, kBlock>>>(input, output, n);
    }
}
