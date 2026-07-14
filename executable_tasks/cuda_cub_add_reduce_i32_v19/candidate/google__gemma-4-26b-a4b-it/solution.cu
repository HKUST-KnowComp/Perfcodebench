#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    // Shared memory for block-level reduction
    extern __shared__ int sdata[];

    int tid = threadIdx.x;
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    int sum = 0;
    // Grid-stride loop to handle arbitrary n
    while (i < n) {
        sum += input[i];
        i += stride;
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

    // Single atomic add per block to global memory
    if (tid == 0) {
        atomicAdd(output, sdata[0]);
    }
}

// Optimized kernel using warp shuffles for even faster reduction
__global__ void reduce_kernel_warp(const int* __restrict__ input, int* __restrict__ output, int n) {
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
    __shared__ int shared_sum[32]; // Max 32 warps in a 1024 thread block
    int lane = threadIdx.x % 32;
    int wid = threadIdx.x / 32;

    if (lane == 0) {
        shared_sum[wid] = sum;
    }
    __syncthreads();

    // Final reduction of warp sums within the block
    if (wid == 0) {
        int final_sum = (threadIdx.x < (blockDim.x / 32)) ? shared_sum[lane] : 0;
        for (int offset = 16; offset > 0; offset >>= 1) {
            final_sum += __shfl_down_sync(0xFFFFFFFF, final_sum, offset);
        }
        if (lane == 0) {
            atomicAdd(output, final_sum);
        }
    }
}

} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    // Determine optimal block/grid size
    // Using 256 threads per block is a good balance for most architectures
    const int threads = 256;
    const int blocks = (n + threads - 1) / threads;
    // Cap blocks to prevent excessive overhead if n is small, 
    // but ensure enough to saturate GPU if n is large.
    const int grid = (blocks > 1024) ? 1024 : blocks;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        // Using the warp-optimized kernel
        reduce_kernel_warp<<<grid, threads>>>(input, output, n);
    }
}