#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_kernel(const int* input, int* output, int n) {
    extern __shared__ int sdata[];
    
    int tid = threadIdx.x;
    int warp_id = tid / 32;
    int lane = tid & 31;
    int global_idx = blockIdx.x * blockDim.x * 4 + threadIdx.x;
    
    // Each thread processes 4 elements in a grid-stride loop
    int sum = 0;
    for (int i = global_idx; i < n; i += blockDim.x * 4 * gridDim.x) {
        if (i < n) sum += input[i];
        if (i + blockDim.x < n) sum += input[i + blockDim.x];
        if (i + 2 * blockDim.x < n) sum += input[i + 2 * blockDim.x];
        if (i + 3 * blockDim.x < n) sum += input[i + 3 * blockDim.x];
    }
    
    // Warp-level reduction using shuffle
    for (int offset = 16; offset > 0; offset >>= 1) {
        sum += __shfl_xor_sync(0xffffffff, sum, offset);
    }
    
    // Write warp sum to shared memory
    if (lane == 0) {
        sdata[warp_id] = sum;
    }
    __syncthreads();
    
    // Reduce shared memory array using a single warp
    if (warp_id == 0) {
        int block_sum = (tid < (blockDim.x / 32)) ? sdata[tid] : 0;
        for (int offset = 16; offset > 0; offset >>= 1) {
            block_sum += __shfl_xor_sync(0xffffffff, block_sum, offset);
        }
        if (tid == 0) {
            atomicAdd(output, block_sum);
        }
    }
}

} // anonymous namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    const int block_size = 256;
    // Each block processes 4*block_size elements per iteration, but grid stride handles remainder
    int grid_size = (n + block_size * 4 - 1) / (block_size * 4);
    if (grid_size < 1) grid_size = 1;
    
    size_t shared_mem_size = (block_size / 32) * sizeof(int);
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<<<grid_size, block_size, shared_mem_size>>>(input, output, n);
    }
}
