#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void optimized_reduce_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    // Use int4 for vectorized loads to maximize bandwidth
    const int4* input_v4 = reinterpret_cast<const int4*>(input);
    int n_v4 = n / 4;
    int sum = 0;

    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    // Process vectorized part
    for (int i = idx; i < n_v4; i += stride) {
        int4 val = input_v4[i];
        sum += val.x + val.y + val.z + val.w;
    }

    // Process remaining elements if n is not a multiple of 4
    if (idx == 0) {
        for (int i = n_v4 * 4; i < n; ++i) {
            sum += input[i];
        }
    }

    // Warp-level reduction
    for (int offset = 16; offset > 0; offset /= 2) {
        sum += __shfl_down_sync(0xffffffff, sum, offset);
    }

    // Block-level reduction using shared memory
    __shared__ int shared_sums[32]; // Max 1024 threads / 32 = 32 warps
    int lane = threadIdx.x % 32;
    int wid = threadIdx.x / 32;

    if (lane == 0) {
        shared_sums[wid] = sum;
    }

    __syncthreads();

    // Final reduction in the first warp
    if (wid == 0) {
        sum = (threadIdx.x < (blockDim.x / 32)) ? shared_sums[lane] : 0;
        for (int offset = 16; offset > 0; offset /= 2) {
            sum += __shfl_down_sync(0xffffffff, sum, offset);
        }
        // One atomic per block
        if (lane == 0) {
            atomicAdd(output, sum);
        }
    }
}

} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    // Heuristic for grid size: enough to saturate the GPU but not too many to cause overhead
    int threads_per_block = 256;
    int blocks_per_grid = (n + (threads_per_block * 4) - 1) / (threads_per_block * 4);
    if (blocks_per_grid > 2048) blocks_per_grid = 2048;
    if (blocks_per_grid == 0) blocks_per_grid = 1;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        if (n > 0) {
            optimized_reduce_kernel<<<blocks_per_grid, threads_per_block>>>(input, output, n);
        }
    }
}