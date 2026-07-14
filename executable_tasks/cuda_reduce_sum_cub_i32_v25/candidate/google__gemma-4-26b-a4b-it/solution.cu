#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_sum_kernel(const int* __restrict__ input, int* output, int n) {
    // Shared memory for block-level reduction
    extern __shared__ int sdata[];

    int tid = threadIdx.x;
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int sum = 0;

    // Grid-stride loop to handle arbitrary n
    while (i < n) {
        sum += input[i];
        i += blockDim.x * gridDim.x;
    }

    sdata[tid] = sum;
    __syncthreads();

    // Warp-level reduction using shuffle
    for (int offset = blockDim.x / 2; offset > 0; offset >>= 1) {
        if (tid < offset) {
            sdata[tid] += sdata[tid + offset];
        }
        __syncthreads();
    }

    // Only the first thread in the block writes to global memory via atomicAdd
    if (tid == 0) {
        atomicAdd(output, sdata[0]);
    }
}

// Optimized kernel using warp shuffles for even faster intra-warp reduction
__global__ void reduce_sum_warp_kernel(const int* __restrict__ input, int* output, int n) {
    int sum = 0;
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    while (i < n) {
        sum += input[i];
        i += stride;
    }

    // Warp reduction
    for (int offset = 16; offset > 0; offset >>= 1) {
        sum += __shfl_down_sync(0xFFFFFFFF, sum, offset);
    }

    // Shared memory to aggregate warp results within the block
    __shared__ int shared_sums[32]; // Max 1024 threads / 32 = 32 warps
    int lane = threadIdx.x % 32;
    int wid = threadIdx.x / 32;

    if (lane == 0) {
        shared_sums[wid] = sum;
    }
    __syncthreads();

    // Final reduction of warp sums within the block
    if (wid == 0) {
        int final_sum = (threadIdx.x < (blockDim.x / 32)) ? shared_sums[lane] : 0;
        for (int offset = 16; offset > 0; offset >>= 1) {
            final_sum += __shfl_down_sync(0xFFFFFFFF, final_sum, offset);
        }
        if (lane == 0) {
            atomicAdd(output, final_sum);
        }
    }
}

} // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    // Tuning parameters
    const int kBlock = 256;
    // Calculate grid size to saturate GPU without excessive overhead
    // For very large n, we use a grid-stride approach.
    const int kGrid = (n + kBlock - 1) / kBlock;
    const int optimizedGrid = (kGrid > 1024) ? 1024 : kGrid;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        // Using the warp-optimized kernel
        reduce_sum_warp_kernel<<<optimizedGrid, kBlock>>>(input, output, n);
    }
}