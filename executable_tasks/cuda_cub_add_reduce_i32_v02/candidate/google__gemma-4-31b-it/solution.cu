#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_kernel(const int* __restrict__ input, int* output, int n) {
    // Use shared memory for block-level reduction
    extern __shared__ int sdata[];

    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x + tid;
    int sum = 0;

    // Grid-stride loop to handle n > total threads
    int stride = blockDim.x * gridDim.x;
    while (idx < n) {
        sum += input[idx];
        idx += stride;
    }

    // Warp reduction
    for (int offset = 16; offset > 0; offset /= 2) {
        sum += __shfl_down_sync(0xFFFFFFFF, sum, offset);
    }

    // Store warp result to shared memory
    if (tid % 32 == 0) {
        sdata[tid / 32] = sum;
    }

    __syncthreads();

    // Final reduction of the shared memory results by the first warp
    if (tid < 32) {
        int final_sum = (tid < (blockDim.x / 32)) ? sdata[tid] : 0;
        for (int offset = 16; offset > 0; offset /= 2) {
            final_sum += __shfl_down_sync(0xFFFFFFFF, final_sum, offset);
        }
        if (tid == 0) {
            atomicAdd(output, final_sum);
        }
    }
}

} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    const int block_size = 256;
    // Calculate grid size to balance occupancy and avoid too many atomic operations
    int grid_size = (n + block_size - 1) / block_size;
    if (grid_size > 1024) grid_size = 1024;

    size_t shared_mem_size = (block_size / 32) * sizeof(int);

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, sizeof(int), 0);
        reduce_kernel<<<grid_size, block_size, shared_mem_size>>>(input, output, n);
    }
}