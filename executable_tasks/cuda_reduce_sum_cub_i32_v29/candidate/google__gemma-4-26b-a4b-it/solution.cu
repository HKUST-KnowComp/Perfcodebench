#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_sum_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
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

    // Single atomic add per block to the global output
    if (tid == 0) {
        atomicAdd(output, sdata[0]);
    }
}

// Optimized kernel using warp shuffles for better performance
__global__ void reduce_sum_warp_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    int sum = 0;
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    while (idx < n) {
        sum += input[idx];
        idx += stride;
    }

    // Warp reduction
    for (int offset = 16; offset > 0; offset /= 2) {
        sum += __shfl_down_sync(0xFFFFFFFF, sum, offset);
    }

    // Shared memory to aggregate warp results within a block
    __shared__ int shared_sums[32]; // Max 1024 threads / 32 = 32 warps
    int lane = threadIdx.x % 32;
    int wid = threadIdx.x / 32;

    if (lane == 0) {
        shared_sums[wid] = sum;
    }

    __syncthreads();

    // Final reduction of warp results in the first warp
    if (wid == 0) {
        int val = (threadIdx.x < (blockDim.x + 31) / 32) ? shared_sums[lane] : 0;
        for (int offset = 16; offset > 0; offset /= 2) {
            val += __shfl_down_sync(0xFFFFFFFF, val, offset);
        }
        if (lane == 0) {
            atomicAdd(output, val);
        }
    }
}

} // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    const int threadsPerBlock = 256;
    // Calculate grid size to balance occupancy and atomic contention
    // We want enough blocks to saturate the GPU but not so many that atomicAdd becomes a bottleneck
    const int grid = (n + threadsPerBlock - 1) / threadsPerBlock;
    const int optimizedGrid = grid > 1024 ? 1024 : grid;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        // Using the warp-shuffle version for maximum performance
        reduce_sum_warp_kernel<<<optimizedGrid, threadsPerBlock>>>(input, output, n);
    }
}