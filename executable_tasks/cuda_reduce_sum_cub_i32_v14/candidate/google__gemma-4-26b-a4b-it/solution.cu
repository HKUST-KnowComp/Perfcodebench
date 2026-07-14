#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_sum_kernel(const int* __restrict__ input, int* output, int n) {
    // Shared memory for block-level reduction
    // Using a size that accommodates the max block size (e.g., 1024)
    extern __shared__ int sdata[];

    int tid = threadIdx.x;
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    int sum = 0;
    // Grid-stride loop to handle n > total threads
    while (i < n) {
        sum += input[i];
        i += stride;
    }

    sdata[tid] = sum;
    __syncthreads();

    // In-block reduction using shared memory
    for (int s = blockDim.x / 2; s > 0; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }

    // Only the first thread in the block writes the partial sum to global memory
    if (tid == 0) {
        atomicAdd(output, sdata[0]);
    }
}

} // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) {
        *output = 0;
        return;
    }

    const int kBlock = 256;
    // Use a grid size that balances occupancy and atomic contention
    // 256 blocks * 256 threads = 65536 threads, usually sufficient for most workloads
    const int kGrid = 256;
    const size_t shared_mem_size = kBlock * sizeof(int);

    for (int iter = 0; iter < iters; ++iter) {
        // Reset output to 0
        cudaMemset(output, 0, sizeof(int));
        
        // Launch kernel with shared memory
        reduce_sum_kernel<<<kGrid, kBlock, shared_mem_size>>>(input, output, n);
    }
}