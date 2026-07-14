#include "interface.h"
#include <cuda_runtime.h>
#include <device_launch_parameters.h>

namespace {

__global__ void reduce_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    // Use shared memory for block-level reduction
    extern __shared__ int sdata[];

    int tid = threadIdx.x;
    int grid_size = blockDim.x * gridDim.x;
    int local_sum = 0;

    // Grid-stride loop to handle arbitrary n
    for (int i = blockIdx.x * blockDim.x + tid; i < n; i += grid_size) {
        local_sum += input[i];
    }

    // Warp-level reduction
    for (int offset = 16; offset > 0; offset /= 2) {
        local_sum += __shfl_down_sync(0xFFFFFFFF, local_sum, offset);
    }

    // Store warp results in shared memory
    if ((tid & 31) == 0) {
        sdata[tid / 32] = local_sum;
    }
    __syncthreads();

    // Final reduction of the 32 warp results by the first warp
    if (tid < 32) {
        int final_sum = (tid < (blockDim.x / 32)) ? sdata[tid] : 0;
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
    if (n <= 0) return;

    // Tuning parameters
    const int block_size = 256;
    const int grid_size = (n + block_size - 1) / block_size;
    // Cap grid size to avoid excessive overhead on small n or too many blocks
    const int limited_grid_size = (grid_size > 1024) ? 1024 : grid_size;
    const int shared_mem_size = (block_size / 32) * sizeof(int);

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, sizeof(int), 0);
        reduce_kernel<<<limited_grid_size, block_size, shared_mem_size>>>(input, output, n);
    }
}