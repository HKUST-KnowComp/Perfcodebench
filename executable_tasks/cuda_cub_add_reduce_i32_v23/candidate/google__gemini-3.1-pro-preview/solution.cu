#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void fast_reduce_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    // Use 64-bit accumulator to prevent overflow during intermediate steps, 
    // though the final result is cast to int32 as per requirements.
    int sum = 0;
    
    // Vectorized loads: process 4 integers at a time per thread
    const int4* input_v4 = reinterpret_cast<const int4*>(input);
    int n_v4 = n / 4;
    
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    // Grid-stride loop for vectorized part
    for (int i = idx; i < n_v4; i += stride) {
        int4 val = input_v4[i];
        sum += val.x + val.y + val.z + val.w;
    }

    // Handle remaining elements if n is not a multiple of 4
    for (int i = n_v4 * 4 + idx; i < n; i += stride) {
        sum += input[i];
    }

    // Warp-level reduction using shuffle primitives
    for (int offset = 16; offset > 0; offset /= 2) {
        sum += __shfl_down_sync(0xFFFFFFFF, sum, offset);
    }

    // Block-level reduction using shared memory
    __shared__ int shared_sums[32]; // Max 32 warps per block (1024 threads)
    int lane = threadIdx.x % 32;
    int warp_id = threadIdx.x / 32;

    if (lane == 0) {
        shared_sums[warp_id] = sum;
    }

    __syncthreads();

    // Final reduction in the first warp
    if (warp_id == 0) {
        sum = (threadIdx.x < (blockDim.x + 31) / 32) ? shared_sums[lane] : 0;
        for (int offset = 16; offset > 0; offset /= 2) {
            sum += __shfl_down_sync(0xFFFFFFFF, sum, offset);
        }
        // Atomic add the block result to global memory
        if (lane == 0) {
            atomicAdd(output, sum);
        }
    }
}

} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    // Heuristic for grid size: aim for high occupancy but avoid excessive blocks
    int block_size = 256;
    int grid_size = (n + (block_size * 4) - 1) / (block_size * 4);
    if (grid_size > 1024) grid_size = 1024;
    if (grid_size < 1) grid_size = 1;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        if (n > 0) {
            fast_reduce_kernel<<<grid_size, block_size>>>(input, output, n);
        }
    }
}