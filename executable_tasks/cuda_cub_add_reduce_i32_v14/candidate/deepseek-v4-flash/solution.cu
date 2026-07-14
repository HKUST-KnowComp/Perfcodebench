#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_sum_kernel(const int* input, int* output, int n) {
    int tid = threadIdx.x;
    int warp_id = tid / 32;
    int lane = tid % 32;
    int global_idx = blockIdx.x * blockDim.x + tid;
    int stride = gridDim.x * blockDim.x;
    
    int sum = 0;
    for (int i = global_idx; i < n; i += stride) {
        sum += input[i];
    }

    // Warp-level reduction using shuffle
    for (int offset = 16; offset > 0; offset >>= 1) {
        sum += __shfl_down_sync(0xFFFFFFFF, sum, offset);
    }

    // Store each warp's partial sum into shared memory
    __shared__ int warp_sums[32]; // supports up to 1024 threads (32 warps)
    if (lane == 0) {
        warp_sums[warp_id] = sum;
    }
    __syncthreads();

    // Reduce warp sums using the first warp
    if (warp_id == 0) {
        int block_sum = (lane < (blockDim.x + 31) / 32) ? warp_sums[lane] : 0;
        for (int offset = 16; offset > 0; offset >>= 1) {
            block_sum += __shfl_down_sync(0xFFFFFFFF, block_sum, offset);
        }
        if (lane == 0) {
            atomicAdd(output, block_sum);
        }
    }
}

} // anonymous namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    const int block_size = 256;
    // Compute grid size: at most 256 blocks to minimize atomic contention
    int grid_size = (n + block_size - 1) / block_size;
    if (grid_size > 256) grid_size = 256;
    // Ensure at least one block
    if (grid_size < 1) grid_size = 1;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_sum_kernel<<<grid_size, block_size>>>(input, output, n);
    }
}
