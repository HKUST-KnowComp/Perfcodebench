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

    // In-block reduction using shuffle instructions
    for (int s = blockDim.x / 2; s > 0; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }

    // Single atomic add per block to the global output
    if (tid == 0) {
        atomicAdd(output, sdata[0]);
    }
}

// Optimized kernel using warp shuffles for even faster reduction
__global__ void reduce_sum_warp_kernel(const int* __restrict__ input, int* output, int n) {
    int sum = 0;
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    while (i < n) {
        sum += input[i];
        i += stride;
    }

    // Warp-level reduction using __shfl_down_sync
    for (int offset = warpSize / 2; offset > 0; offset /= 2) {
        sum += __shfl_down_sync(0xFFFFFFFF, sum, offset);
    }

    // Shared memory to reduce results from all warps in the block
    __shared__ int shared_sums[32]; // Max 1024 threads / 32 = 32 warps
    int lane = threadIdx.x % warpSize;
    int wid = threadIdx.x / warpSize;

    if (lane == 0) {
        shared_sums[wid] = sum;
    }

    __syncthreads();

    // Final reduction of the shared_sums array
    // Only the first warp handles this
    if (wid == 0) {
        int final_sum = (threadIdx.x < (blockDim.x / warpSize)) ? shared_sums[lane] : 0;
        for (int offset = warpSize / 2; offset > 0; offset /= 2) {
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
    // Calculate grid size to ensure we cover n with grid-stride, 
    // but keep grid size reasonable to avoid excessive atomic contention.
    // A grid size of 128-256 is usually a sweet spot for int32 reduction.
    const int kGrid = (n + kBlock - 1) / kBlock;
    const int optimizedGrid = kGrid < 256 ? kGrid : 256;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_sum_warp_kernel<<<optimizedGrid, kBlock>>>(input, output, n);
    }
}