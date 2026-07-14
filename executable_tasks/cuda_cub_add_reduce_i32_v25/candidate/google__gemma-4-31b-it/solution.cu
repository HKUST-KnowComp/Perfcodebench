#include "interface.h"
#include <cuda_runtime.h>
#include <device_launch_parameters.h>

namespace {

__global__ void reduce_sum_kernel(const int* __restrict__ input, int* output, int n) {
    // Use shared memory for block-level reduction
    __shared__ int sdata[32];

    int tid = threadIdx.x;
    int grid_stride = blockDim.x * gridDim.x;
    int local_sum = 0;

    // Grid-stride loop to handle any n
    for (int i = blockIdx.x * blockDim.x + tid; i < n; i += grid_stride) {
        local_sum += input[i];
    }

    // Warp-level reduction
    for (int offset = 16; offset > 0; offset /= 2) {
        local_sum += __shfl_down_sync(0xFFFFFFFF, local_sum, offset);
    }

    // Only the first thread of each warp writes to shared memory
    if ((tid & 31) == 0) {
        sdata[tid / 32] = local_sum;
    }

    __syncthreads();

    // Final reduction of the 32 warp sums by the first warp
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
    // Tuning parameters: 256 threads per block, 128 blocks (total 32k threads)
    // This provides a good balance between occupancy and atomic contention
    const int threads_per_block = 256;
    const int blocks_per_grid = 128;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, sizeof(int), 0);
        reduce_sum_kernel<<<blocks_per_grid, threads_per_block>>>(input, output, n);
    }
}