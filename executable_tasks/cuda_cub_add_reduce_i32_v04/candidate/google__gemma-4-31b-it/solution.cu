#include "interface.h"
#include <cuda_runtime.h>
#include <device_launch_parameters.h>

namespace {

__global__ void reduce_kernel(const int* __restrict__ input, int* output, int n) {
    // Use shared memory for block-level reduction
    __shared__ int sdata[32];

    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    
    int local_sum = 0;
    // Grid-stride loop to handle n > total threads
    int stride = blockDim.x * gridDim.x;
    while (idx < n) {
        local_sum += input[idx];
        idx += stride;
    }

    // Warp reduction
    for (int offset = 16; offset > 0; offset /= 2) {
        local_sum += __shfl_down_sync(0xFFFFFFFF, local_sum, offset);
    }

    // Store warp results in shared memory
    if ((tid & 31) == 0) {
        sdata[blockIdx.x % 32] = 0; // This is a placeholder, we need a different approach for block sum
    }
    
    // Correct block reduction: only the first thread of each warp writes to shared memory
    // But since we have 256 threads (8 warps), we can just use a small shared array
    __shared__ int block_sums[8];
    if ((tid & 31) == 0) {
        block_sums[tid / 32] = local_sum;
    }
    __syncthreads();

    // Final reduction of the 8 warp sums by the first warp
    if (tid < 8) {
        int final_sum = block_sums[tid];
        // Since it's only 8 elements, we can just use a simple loop or shfl
        // But for simplicity and correctness in a small set:
        if (tid == 0) {
            int total = 0;
            for(int i=0; i<8; ++i) total += block_sums[i];
            atomicAdd(output, total);
        }
    }
}

// A more robust version of the kernel
__global__ void reduce_kernel_v2(const int* __restrict__ input, int* output, int n) {
    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int local_sum = 0;
    int stride = blockDim.x * gridDim.x;
    
    while (idx < n) {
        local_sum += input[idx];
        idx += stride;
    }

    // Warp shuffle reduction
    for (int offset = 16; offset > 0; offset /= 2) {
        local_sum += __shfl_down_sync(0xFFFFFFFF, local_sum, offset);
    }

    static __shared__ int shared_sums[32];
    int lane = tid % 32;
    int wid = tid / 32;

    if (lane == 0) {
        shared_sums[wid] = local_sum;
    }
    __syncthreads();

    // Final reduction in the first warp
    if (wid == 0) {
        int final_sum = (tid < 8) ? shared_sums[tid] : 0;
        for (int offset = 4; offset > 0; offset /= 2) {
            final_sum += __shfl_down_sync(0xFFFFFFFF, final_sum, offset);
        }
        if (tid == 0) {
            atomicAdd(output, final_sum);
        }
    }
}

} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    // Tuning: 256 threads per block, enough blocks to saturate GPU
    int threadsPerBlock = 256;
    int blocksPerGrid = (n + threadsPerBlock - 1) / threadsPerBlock;
    if (blocksPerGrid > 2048) blocksPerGrid = 2048;
    if (n == 0) return;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        // Using v2 for better warp-level handling
        reduce_kernel_v2<<<blocksPerGrid, threadsPerBlock>>>(input, output, n);
    }
}