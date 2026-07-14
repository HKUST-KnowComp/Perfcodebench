#include "interface.h"
#include <cuda_runtime.h>

namespace {
constexpr int BLOCK_SIZE = 256;

/**
 * Parallel reduction kernel.
 * Reduces the n-element input array to a single value stored in output.
 * Uses shared memory and tree reduction within each block.
 */
__global__ void reduce_kernel(const float* __restrict__ input,
                              float* __restrict__ output,
                              int n) {
    __shared__ double sdata[BLOCK_SIZE];

    // Thread index within block and global index
    int tid = threadIdx.x;
    int bid = blockIdx.x;
    int blockDim_ = blockDim.x;
    int gridDim_ = gridDim.x;

    // Each thread accumulates a partial sum using grid-stride loop
    double sum = 0.0;
    for (int i = bid * blockDim_ + tid; i < n; i += gridDim_ * blockDim_) {
        sum += static_cast<double>(input[i]);
    }

    // Store partial sum to shared memory
    sdata[tid] = sum;
    __syncthreads();

    // In-warp reduction using shuffle (if blockDim is multiple of warpSize)
    for (int offset = blockDim_ / 2; offset > 0; offset >>= 1) {
        if (tid < offset) {
            sdata[tid] += sdata[tid + offset];
        }
        __syncthreads();
    }

    // First thread writes result
    if (tid == 0) {
        output[bid] = static_cast<float>(sdata[0]);
    }
}
} // anonymous namespace

float reduce_f32(const float* input, int n, int iters) {
    if (n == 0) return 0.0f;

    float* d_output = nullptr;
    float* d_block_sums = nullptr;

    // Compute number of blocks (capped to avoid excessive launches)
    int numBlocks = (n + BLOCK_SIZE - 1) / BLOCK_SIZE;
    if (numBlocks > 512) numBlocks = 512;  // typical GPU has ~80 SMs, 512 is safe

    cudaMalloc(&d_output, sizeof(float));
    cudaMalloc(&d_block_sums, numBlocks * sizeof(float));

    // Repeatedly perform reduction (for benchmarking)
    for (int iter = 0; iter < iters; ++iter) {
        // Phase 1: reduce input to block sums
        reduce_kernel<<<numBlocks, BLOCK_SIZE>>>(input, d_block_sums, n);
        // Phase 2: reduce block sums to a single scalar
        reduce_kernel<<<1, BLOCK_SIZE>>>(d_block_sums, d_output, numBlocks);
        cudaDeviceSynchronize();  // ensure completion before next iteration
    }

    // Copy final result back to host
    float result;
    cudaMemcpy(&result, d_output, sizeof(float), cudaMemcpyDeviceToHost);

    cudaFree(d_block_sums);
    cudaFree(d_output);

    return result;
}
