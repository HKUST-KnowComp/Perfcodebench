#include "interface.h"
#include <cuda_runtime.h>
#include <device_launch_parameters.h>

namespace {

__global__ void reduce_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    // Use shared memory for block-level reduction
    extern __shared__ int sdata[];

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

    // Final reduction of the 8 warp results (for blockDim=256)
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
    if (n <= 0) return;

    // Configuration: 256 threads per block, enough blocks to saturate GPU
    const int threadsPerBlock = 256;
    const int blocksPerGrid = (n + threadsPerBlock - 1) / threadsPerBlock;
    // Cap blocks to avoid excessive overhead on small n or too many blocks on large n
    const int gridSize = (blocksPerGrid < 1024) ? blocksPerBlock : 1024;
    const int sharedMemSize = (threadsPerBlock / 32) * sizeof(int);

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, sizeof(int), 0);
        reduce_kernel<<<gridSize, threadsPerBlock, sharedMemSize>>>(input, output, n);
    }
}