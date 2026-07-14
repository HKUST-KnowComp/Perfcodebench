#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_block_kernel(const int* input, int* output, int n) {
    extern __shared__ int shared[];
    int tid = threadIdx.x;
    int gid = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    // Grid-stride loop to accumulate elements assigned to this thread
    int sum = 0;
    for (int i = gid; i < n; i += stride) {
        sum += input[i];
    }

    // Warp-level reduction using shuffle
    int lane = tid % warpSize;
    int warp_id = tid / warpSize;
    for (int offset = warpSize / 2; offset > 0; offset >>= 1) {
        sum += __shfl_down_sync(0xFFFFFFFF, sum, offset);
    }
    // Each warp leader writes its partial sum to shared memory
    if (lane == 0) {
        shared[warp_id] = sum;
    }
    __syncthreads();

    // Reduction of partial sums across warps (now within first warp)
    int num_warps = (blockDim.x + warpSize - 1) / warpSize;
    if (tid < num_warps) {
        int val = shared[tid];
        for (int offset = warpSize / 2; offset > 0; offset >>= 1) {
            val += __shfl_down_sync(0xFFFFFFFF, val, offset);
        }
        // First thread of each block atomically adds its block's sum to global output
        if (lane == 0) {
            atomicAdd(output, val);
        }
    }
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    constexpr int kBlock = 256;
    // Compute grid size to cover all elements, capped to avoid launching too many blocks
    int grid = (n + kBlock - 1) / kBlock;
    if (grid > 1024) grid = 1024;  // Limit to 1024 blocks as in baseline, but dynamic when n is small

    int shared_mem_size = ((kBlock + warpSize - 1) / warpSize) * sizeof(int);

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_block_kernel<<<grid, kBlock, shared_mem_size>>>(input, output, n);
    }
}
