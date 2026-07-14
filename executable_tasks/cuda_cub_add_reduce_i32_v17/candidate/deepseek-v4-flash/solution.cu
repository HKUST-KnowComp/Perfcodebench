#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_kernel(const int* input, int* output, int n) {
    const int warpSize = 32;
    int lane = threadIdx.x % warpSize;
    int warp_id = threadIdx.x / warpSize;
    int warp_sum = 0;

    // Grid-stride loop to load elements
    for (int i = blockIdx.x * blockDim.x + threadIdx.x; i < n; i += gridDim.x * blockDim.x) {
        warp_sum += input[i];
    }

    // Warp-level reduction using shfl_down_sync
    for (int offset = warpSize / 2; offset > 0; offset >>= 1) {
        warp_sum += __shfl_down_sync(0xFFFFFFFF, warp_sum, offset);
    }

    // First lane of each warp writes to shared memory
    extern __shared__ int sdata[];
    if (lane == 0) {
        sdata[warp_id] = warp_sum;
    }
    __syncthreads();

    // Final reduction over warp sums in shared memory
    if (warp_id == 0) {
        int block_sum = (lane < blockDim.x / warpSize) ? sdata[lane] : 0;
        for (int offset = warpSize / 2; offset > 0; offset >>= 1) {
            block_sum += __shfl_down_sync(0xFFFFFFFF, block_sum, offset);
        }
        if (lane == 0) {
            atomicAdd(output, block_sum);
        }
    }
}

}  // anonymous namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    const int blockDim = 256;
    const int numBlocks = 1024;  // fixed number of blocks for occupancy
    const size_t sharedMem = (blockDim / 32) * sizeof(int);

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<<<numBlocks, blockDim, sharedMem>>>(input, output, n);
    }
}