#include "interface.h"
#include <cuda_runtime.h>
#include <device_launch_parameters.h>

namespace {

__global__ void reduce_sum_kernel(const int* __restrict__ input, int* output, int n) {
    // Use shared memory for block-level reduction
    __shared__ int sdata[32];

    int tid = threadIdx.x;
    int global_tid = blockIdx.x * blockDim.x + threadIdx.x;
    int sum = 0;

    // Grid-stride loop to handle n > total threads
    for (int i = global_tid; i < n; i += blockDim.x * gridDim.x) {
        sum += input[i];
    }

    // Warp-level reduction
    for (int offset = 16; offset > 0; offset /= 2) {
        sum += __shfl_down_sync(0xFFFFFFFF, sum, offset);
    }

    // Store warp results to shared memory
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
    // Tuning parameters
    const int blockSize = 256;
    // Calculate grid size to balance occupancy and atomic contention
    // 128 blocks * 256 threads = 32k threads, usually sufficient to saturate memory bandwidth
    const int gridSize = (n + blockSize - 1) / blockSize;
    const int limitedGridSize = (gridSize > 128) ? 128 : gridSize;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, sizeof(int), 0);
        if (n > 0) {
            reduce_sum_kernel<<<limitedGridSize, blockSize>>>(input, output, n);
        }
    }
}