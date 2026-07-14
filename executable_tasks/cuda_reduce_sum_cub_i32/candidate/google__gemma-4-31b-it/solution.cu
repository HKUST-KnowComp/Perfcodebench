#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_sum_kernel(const int* __restrict__ input, int* output, int n) {
    // Use shared memory for block-level reduction
    extern __shared__ int sdata[];

    int tid = threadIdx.x;
    int block_sum = 0;

    // Grid-stride loop to handle arbitrary n
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

    // Final reduction of the 8 warp sums (for blockDim = 256)
    if (tid == 0) {
        int final_block_sum = 0;
        for (int i = 0; i < (blockDim.x / 32); ++i) {
            final_block_sum += sdata[i];
        }
        atomicAdd(output, final_block_sum);
    }
}

} // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    const int kBlockSize = 256;
    // Calculate grid size to saturate GPU while keeping it reasonable
    // 1024 blocks is usually sufficient for most modern GPUs
    const int kGridSize = (n + kBlockSize - 1) / kBlockSize > 1024 ? 1024 : (n + kBlockSize - 1) / kBlockSize;
    const int sharedMemSize = (kBlockSize / 32) * sizeof(int);

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, sizeof(int), 0);
        reduce_sum_kernel<<<kGridSize, kBlockSize, sharedMemSize>>>(input, output, n);
    }
}