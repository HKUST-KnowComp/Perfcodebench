#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void reduce_sum_kernel(const int* __restrict__ input, int* output, int n) {
    // Shared memory for warp-level partial sums
    extern __shared__ int sdata[];
    const int tid = threadIdx.x;
    const int blockSize = blockDim.x;
    const int warpSize = 32;
    const int warpId = tid / warpSize;
    const int lane = tid & (warpSize - 1);

    // Each thread computes its partial sum over a grid-stride loop
    int sum = 0;
    for (int idx = blockIdx.x * blockSize + tid; idx < n; idx += blockSize * gridDim.x) {
        sum += input[idx];
    }

    // Reduce within warp using shuffle
    #pragma unroll
    for (int offset = warpSize / 2; offset > 0; offset >>= 1) {
        sum += __shfl_down_sync(0xffffffff, sum, offset);
    }

    // Write warp sum to shared memory
    if (lane == 0) {
        sdata[warpId] = sum;
    }
    __syncthreads();

    // First warp reduces the warp sums
    if (warpId == 0) {
        // Load the relevant warp sum (zero for warps beyond the number of active warps)
        sum = (tid < (blockSize / warpSize)) ? sdata[tid] : 0;
        #pragma unroll
        for (int offset = warpSize / 2; offset > 0; offset >>= 1) {
            sum += __shfl_down_sync(0xffffffff, sum, offset);
        }
        // First thread of first warp adds block sum to global output
        if (tid == 0) {
            atomicAdd(output, sum);
        }
    }
}

int get_sm_count() {
    static int sm_count = 0;
    if (sm_count == 0) {
        cudaDeviceProp prop;
        cudaGetDeviceProperties(&prop, 0);
        sm_count = prop.multiProcessorCount;
    }
    return sm_count;
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    constexpr int kBlockSize = 256;
    const int smCount = get_sm_count();
    // Heuristic: use 8 warps per SM to hide latency, but cap at a reasonable number of blocks.
    // For n = 0, no kernel launch needed.
    int grid = (n == 0) ? 0 : std::min(8 * smCount, (n + kBlockSize - 1) / kBlockSize);
    grid = std::max(grid, 1); // ensure at least one block if n > 0

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        if (grid > 0) {
            reduce_sum_kernel<<<grid, kBlockSize, kBlockSize * sizeof(int)>>>(
                input, output, n);
        }
    }
}
