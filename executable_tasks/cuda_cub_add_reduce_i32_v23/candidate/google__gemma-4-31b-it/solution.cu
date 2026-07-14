#include "interface.h"
#include <cuda_runtime.h>
#include <device_launch_parameters.h>

namespace {

__global__ void reduce_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    // Use shared memory for block-level reduction
    __shared__ int sdata[32];

    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int sum = 0;

    // Grid-stride loop to handle n > total threads
    int grid_size = blockDim.x * gridDim.x;
    for (int i = idx; i < n; i += grid_size) {
        sum += input[i];
    }

    // Warp-level reduction
    for (int offset = 16; offset > 0; offset /= 2) {
        sum += __shfl_down_sync(0xFFFFFFFF, sum, offset);
    }

    // Store warp results in shared memory
    if ((tid & 31) == 0) {
        sdata[tid / 32] = sum;
    }

    __syncthreads();

    // Final reduction of the 32 warp results by the first warp
    if (tid < 32) {
        int final_sum = sdata[tid];
        for (int offset = 16; offset > 0; offset /= 2) {
            final_sum += __shfl_down_sync(0xFFFFFFFF, final_sum, offset);
        }
        if (tid == 0) {
            atomicAdd(output, final_sum);
        }
    }
}

} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    // Tuning parameters: 256 threads per block, enough blocks to saturate GPU
    const int threads_per_block = 256;
    const int blocks_per_grid = (n + threads_per_block - 1) / threads_per_block;
    // Cap blocks to avoid excessive overhead for very large n, as grid-stride loop handles it
    const int capped_blocks = (blocks_per_grid > 1024) ? 1024 : blocks_per_grid;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, sizeof(int), 0);
        reduce_kernel<<<capped_blocks, threads_per_block>>>(input, output, n);
    }
}