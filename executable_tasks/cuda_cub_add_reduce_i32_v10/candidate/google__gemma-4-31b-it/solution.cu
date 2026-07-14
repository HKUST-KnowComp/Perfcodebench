#include "interface.h"
#include <cuda_runtime.h>
#include <device_launch_parameters.h>

namespace {

__global__ void reduce_kernel(const int* __restrict__ input, int* output, int n) {
    // Use shared memory for block-level reduction
    extern __shared__ int sdata[];

    int tid = threadIdx.x;
    int block_offset = blockIdx.x * blockDim.x;
    int grid_size = blockDim.x * gridDim.x;

    int local_sum = 0;
    // Grid-stride loop to handle n > total threads
    for (int i = block_offset + tid; i < n; i += grid_size) {
        local_sum += input[i];
    }

    // Warp reduction
    for (int offset = 16; offset > 0; offset /= 2) {
        local_sum += __shfl_down_sync(0xFFFFFFFF, local_sum, offset);
    }

    // Store warp results to shared memory
    if ((tid & 31) == 0) {
        sdata[tid / 32] = local_sum;
    }
    __syncthreads();

    // Final reduction of the 8 warp results (for 256 threads)
    if (tid == 0) {
        int block_sum = 0;
        for (int i = 0; i < (blockDim.x / 32); ++i) {
            block_sum += sdata[i];
        }
        atomicAdd(output, block_sum);
    }
}

} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    const int threads_per_block = 256;
    // Calculate grid size to saturate GPU while keeping it reasonable
    int grid_size = (n + threads_per_block - 1) / threads_per_block;
    if (grid_size > 1024) grid_size = 1024;
    if (grid_size == 0 && n > 0) grid_size = 1;

    size_t shared_mem_size = (threads_per_block / 32) * sizeof(int);

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, sizeof(int), 0);
        if (n > 0) {
            reduce_kernel<<<grid_size, threads_per_block, shared_mem_size>>>(input, output, n);
        }
    }
}