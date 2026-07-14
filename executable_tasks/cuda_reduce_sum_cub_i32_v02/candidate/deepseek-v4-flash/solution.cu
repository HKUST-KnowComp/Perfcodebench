// interface.h is expected to declare:
// void reduce_sum_i32(const int* input, int* output, int n, int iters);

#include "interface.h"
#include <cuda_runtime.h>

namespace {

// Block size can be tuned; 256 is a common choice.
constexpr int kBlockSize = 256;

// Maximum number of blocks to launch. Sufficient to saturate SMs without excessive atomicAdd.
constexpr int kMaxBlocks = 1024;

// Kernel: each block reduces its assigned portion of the input, then does a single atomicAdd.
__global__ void reduce_sum_kernel(const int* input, int* output, int n) {
    __shared__ int sdata[kBlockSize];

    int tid = threadIdx.x;
    int block_start = blockIdx.x * blockDim.x;
    int global_stride = gridDim.x * blockDim.x;

    // Each thread accumulates multiple elements with a stride loop.
    int sum = 0;
    for (int idx = block_start + tid; idx < n; idx += global_stride) {
        sum += input[idx];
    }

    // Store the partial sum in shared memory.
    sdata[tid] = sum;
    __syncthreads();

    // Reduction in shared memory.
    // We assume blockDim.x is a power of two (which it is).
    for (int offset = blockDim.x / 2; offset > 0; offset >>= 1) {
        if (tid < offset) {
            sdata[tid] += sdata[tid + offset];
        }
        __syncthreads();
    }

    // Thread 0 writes the block sum to output using atomicAdd.
    if (tid == 0) {
        atomicAdd(output, sdata[0]);
    }
}

}  // anonymous namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    // Compute grid size: enough blocks to cover n, but cap to avoid overhead.
    int required_blocks = (n + kBlockSize - 1) / kBlockSize;
    int grid = min(required_blocks, kMaxBlocks);
    if (grid < 1) grid = 1;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_sum_kernel<<<grid, kBlockSize>>>(input, output, n);
    }
}
