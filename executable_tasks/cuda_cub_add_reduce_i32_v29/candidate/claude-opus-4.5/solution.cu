#include "interface.h"
#include <cuda_runtime.h>

namespace {

// Warp-level reduction using shuffle
__device__ __forceinline__ int warp_reduce(int val) {
    #pragma unroll
    for (int offset = 16; offset > 0; offset >>= 1) {
        val += __shfl_down_sync(0xffffffff, val, offset);
    }
    return val;
}

// Block-level reduction kernel - each block produces one partial sum
__global__ void block_reduce_kernel(const int* __restrict__ input, int* __restrict__ partial, int n) {
    extern __shared__ int sdata[];
    
    const int tid = threadIdx.x;
    const int bid = blockIdx.x;
    const int bdim = blockDim.x;
    const int grid_stride = bdim * gridDim.x;
    int idx = bid * bdim + tid;
    
    // Grid-stride loop accumulation
    int sum = 0;
    while (idx < n) {
        sum += input[idx];
        idx += grid_stride;
    }
    
    // Warp-level reduction first
    const int lane = tid & 31;
    const int wid = tid >> 5;
    sum = warp_reduce(sum);
    
    // Write warp results to shared memory
    if (lane == 0) {
        sdata[wid] = sum;
    }
    __syncthreads();
    
    // Final reduction by first warp
    const int num_warps = bdim >> 5;
    if (tid < num_warps) {
        sum = sdata[tid];
    } else {
        sum = 0;
    }
    
    if (wid == 0) {
        sum = warp_reduce(sum);
        if (tid == 0) {
            partial[bid] = sum;
        }
    }
}

// Final reduction kernel - reduces partial sums to single output
__global__ void final_reduce_kernel(const int* __restrict__ partial, int* __restrict__ output, int num_blocks) {
    extern __shared__ int sdata[];
    
    const int tid = threadIdx.x;
    const int bdim = blockDim.x;
    
    // Load partial sums
    int sum = 0;
    for (int i = tid; i < num_blocks; i += bdim) {
        sum += partial[i];
    }
    
    // Warp-level reduction
    const int lane = tid & 31;
    const int wid = tid >> 5;
    sum = warp_reduce(sum);
    
    if (lane == 0) {
        sdata[wid] = sum;
    }
    __syncthreads();
    
    const int num_warps = bdim >> 5;
    if (tid < num_warps) {
        sum = sdata[tid];
    } else {
        sum = 0;
    }
    
    if (wid == 0) {
        sum = warp_reduce(sum);
        if (tid == 0) {
            *output = sum;
        }
    }
}

static int* d_partial = nullptr;
static int partial_capacity = 0;

}  // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) {
        cudaMemset(output, 0, sizeof(int));
        return;
    }
    
    const int block_size = 256;
    const int max_blocks = 1024;
    int num_blocks = (n + block_size - 1) / block_size;
    if (num_blocks > max_blocks) num_blocks = max_blocks;
    
    // Allocate partial sums buffer if needed
    if (num_blocks > partial_capacity) {
        if (d_partial) cudaFree(d_partial);
        cudaMalloc(&d_partial, num_blocks * sizeof(int));
        partial_capacity = num_blocks;
    }
    
    const int smem_size = (block_size / 32) * sizeof(int);
    
    for (int iter = 0; iter < iters; ++iter) {
        // Phase 1: Block-level reduction
        block_reduce_kernel<<<num_blocks, block_size, smem_size>>>(input, d_partial, n);
        
        // Phase 2: Final reduction of partial sums
        final_reduce_kernel<<<1, block_size, smem_size>>>(d_partial, output, num_blocks);
    }
}