#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_sum_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    // Shared memory for block-level reduction
    extern __shared__ int sdata[];

    int tid = threadIdx.x;
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    int local_sum = 0;

    // Grid-stride loop to handle arbitrary n
    while (i < n) {
        local_sum += input[i];
        i += stride;
    }

    sdata[tid] = local_sum;
    __syncthreads();

    // In-block reduction using warp shuffle
    for (int offset = blockDim.x / 2; offset > 0; offset >>= 1) {
        if (tid < offset) {
            sdata[tid] += sdata[tid + offset];
        }
        __syncthreads();
    }

    // Single atomic add per block to global memory
    if (tid == 0) {
        atomicAdd(output, sdata[0]);
    }
}

// Optimized version using warp shuffles for faster intra-warp reduction
__global__ void reduce_sum_warp_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    int local_sum = 0;
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    while (i < n) {
        local_sum += input[i];
        i += stride;
    }

    // Warp reduction using shuffle
    for (int offset = 16; offset > 0; offset >>= 1) {
        local_sum += __shfl_down_sync(0xFFFFFFFF, local_sum, offset);
    }

    // Shared memory to aggregate warp results within a block
    __shared__ int shared_sums[32]; // Max 1024 threads / 32 = 32 warps
    int lane = threadIdx.x % 32;
    int wid = threadIdx.x / 32;

    if (lane == 0) {
        shared_sums[wid] = local_sum;
    }
    __syncthreads();

    // Final reduction of warp results in the first warp
    if (wid == 0) {
        int final_lane_sum = (threadIdx.x < (blockDim.x / 32)) ? shared_sums[lane] : 0;
        for (int offset = 16; offset > 0; offset >>= 1) {
            final_lane_sum += __shfl_down_sync(0xFFFFFFFF, final_lane_sum, offset);
        }
        if (lane == 0) {
            atomicAdd(output, final_lane_sum);
        }
    }
}

} // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    // Tuning parameters
    const int kBlock = 256;
    // Calculate grid size to saturate SMs without excessive overhead
    // For large N, we use a grid-stride approach. 
    // A fixed grid size is often more stable for performance metrics.
    const int kGrid = 256; 

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_sum_warp_kernel<<<kGrid, kBlock>>>(input, output, n);
    }
}