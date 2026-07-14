#include "interface.h"
#include <cuda_runtime.h>

namespace {

template <int blockSize>
__global__ void reduce_kernel(const int* input, int* output, int n) {
    // Shared memory for per-warp partial sums
    __shared__ int shared[blockSize / 32];
    
    int tid = threadIdx.x;
    int idx = blockIdx.x * blockSize + tid;
    
    // Grid-stride loop to accumulate multiple elements per thread
    int sum = 0;
    int stride = gridDim.x * blockSize;
    for (int i = idx; i < n; i += stride) {
        sum += input[i];
    }
    
    // Warp-level reduction using __shfl_down_sync
    #pragma unroll
    for (int offset = 16; offset > 0; offset >>= 1) {
        sum += __shfl_down_sync(0xffffffff, sum, offset);
    }
    
    // First lane of each warp writes to shared memory
    if (tid % 32 == 0) {
        shared[tid / 32] = sum;
    }
    __syncthreads();
    
    // Second-level reduction in first warp
    if (tid < (blockSize / 32)) {
        int val = shared[tid];
        #pragma unroll
        for (int offset = (blockSize / 32) / 2; offset > 0; offset >>= 1) {
            if (tid < offset) {
                val += shared[tid + offset];
                shared[tid] = val;
            }
            __syncthreads();
        }
        if (tid == 0) {
            atomicAdd(output, shared[0]);
        }
    }
}

} // anonymous namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    const int blockSize = 256;
    const int gridSize = (n + blockSize - 1) / blockSize;
    
    // Zero output once before all iterations
    cudaMemset(output, 0, sizeof(int));
    
    for (int iter = 0; iter < iters; ++iter) {
        reduce_kernel<blockSize><<<gridSize, blockSize>>>(input, output, n);
    }
}
