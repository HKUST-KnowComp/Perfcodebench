#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int BLOCK_SIZE = 256;
constexpr int ITEMS_PER_THREAD = 4;

__global__ void parallel_reduce_kernel(const float* input, float* output, int n) {
    __shared__ float sdata[BLOCK_SIZE];
    
    int tid = threadIdx.x;
    int idx = blockIdx.x * (BLOCK_SIZE * ITEMS_PER_THREAD) + threadIdx.x;
    
    // Each thread accumulates multiple elements
    float sum = 0.0f;
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; ++i) {
        int offset = idx + i * BLOCK_SIZE;
        if (offset < n) {
            sum += input[offset];
        }
    }
    
    sdata[tid] = sum;
    __syncthreads();
    
    // Block-level reduction in shared memory
    for (int s = BLOCK_SIZE / 2; s > 32; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }
    
    // Warp-level reduction (no sync needed)
    if (tid < 32) {
        volatile float* vdata = sdata;
        if (BLOCK_SIZE >= 64) vdata[tid] += vdata[tid + 32];
        vdata[tid] += vdata[tid + 16];
        vdata[tid] += vdata[tid + 8];
        vdata[tid] += vdata[tid + 4];
        vdata[tid] += vdata[tid + 2];
        vdata[tid] += vdata[tid + 1];
    }
    
    // First thread writes block result
    if (tid == 0) {
        atomicAdd(output, sdata[0]);
    }
}

}  // namespace

float reduce_f32(const float* input, int n, int iters) {
    float* d_out = nullptr;
    cudaMalloc(&d_out, sizeof(float));
    
    int grid_size = (n + BLOCK_SIZE * ITEMS_PER_THREAD - 1) / (BLOCK_SIZE * ITEMS_PER_THREAD);
    
    float actual = 0.0f;
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(d_out, 0, sizeof(float));
        parallel_reduce_kernel<<<grid_size, BLOCK_SIZE>>>(input, d_out, n);
    }
    
    cudaMemcpy(&actual, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_out);
    return actual;
}