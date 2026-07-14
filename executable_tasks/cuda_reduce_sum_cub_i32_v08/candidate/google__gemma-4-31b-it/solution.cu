#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_sum_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    // Use shared memory for block-level reduction
    extern __shared__ int sdata[];

    int tid = threadIdx.x;
    int block_sum = 0;

    // Grid-stride loop to process all elements
    for (int i = blockIdx.x * blockDim.x + tid; i < n; i += blockDim.x * gridDim.x) {
        block_sum += input[i];
    }

    // Warp-level reduction
    for (int offset = 16; offset > 0; offset /= 2) {
        block_sum += __shfl_down_sync(0xFFFFFFFF, block_sum, offset);
    }

    // Store warp results in shared memory
    if ((tid & 31) == 0) {
        sdata[tid / 32] = block_sum;
    }

    __syncthreads();

    // Final reduction of the shared memory results by the first warp
    if (tid < 32) {
        int final_block_sum = (tid < (blockDim.x / 32)) ? sdata[tid] : 0;
        for (int offset = 16; offset > 0; offset /= 2) {
            final_block_sum += __shfl_down_sync(0xFFFFFFFF, final_block_sum, offset);
        }
        if (tid == 0) {
            atomicAdd(output, final_block_sum);
        }
    }
}

} // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    const int block_size = 256;
    // Calculate grid size to saturate GPU (approx 2-4 blocks per SM)
    int grid_size = (n + block_size - 1) / block_size;
    if (grid_size > 1024) grid_size = 1024;

    // Shared memory size: one int per warp
    size_t shared_mem_size = (block_size / 32) * sizeof(int);

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, sizeof(int), 0);
        reduce_sum_kernel<<<grid_size, block_size, shared_mem_size>>>(input, output, n);
    }
}