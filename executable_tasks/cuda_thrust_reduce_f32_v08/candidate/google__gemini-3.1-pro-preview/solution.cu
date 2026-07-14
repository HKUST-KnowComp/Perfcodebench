#include "interface.h"
#include <cuda_runtime.h>

namespace {
/**
 * Optimized reduction kernel using grid-stride loops and warp shuffles.
 * Each thread accumulates multiple elements into a local sum, then performs
 * a warp-level reduction using __shfl_down_sync, and finally uses atomicAdd
 * to contribute the warp's result to the global output.
 */
__global__ void fast_reduce_kernel(const float* __restrict__ input, float* __restrict__ output, int n) {
    float sum = 0.0f;
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    // Grid-stride loop to process all elements
    for (int i = tid; i < n; i += stride) {
        sum += input[i];
    }

    // Warp-level reduction using shuffle instructions
    unsigned int mask = 0xFFFFFFFF;
    for (int offset = 16; offset > 0; offset /= 2) {
        sum += __shfl_down_sync(mask, sum, offset);
    }

    // The first thread of each warp adds the warp sum to the global output
    if ((threadIdx.x & 31) == 0) {
        atomicAdd(output, sum);
    }
}
}  // namespace

float reduce_f32(const float* input, int n, int iters) {
    if (n <= 0) return 0.0f;

    float* d_out = nullptr;
    cudaMalloc(&d_out, sizeof(float));

    // Heuristic for grid size: aim for high occupancy without excessive overhead
    int threads_per_block = 256;
    int blocks_per_grid = (n + threads_per_block - 1) / threads_per_block;
    if (blocks_per_grid > 1024) blocks_per_grid = 1024;

    float actual = 0.0f;
    for (int iter = 0; iter < iters; ++iter) {
        // Reset the output for each iteration
        cudaMemset(d_out, 0, sizeof(float));
        
        // Launch the parallel reduction kernel
        fast_reduce_kernel<<<blocks_per_grid, threads_per_block>>>(input, d_out, n);
    }

    // Copy the final result back to host
    cudaMemcpy(&actual, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_out);

    return actual;
}