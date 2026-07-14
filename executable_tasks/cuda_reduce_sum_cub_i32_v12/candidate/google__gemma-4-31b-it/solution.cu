#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

namespace {

__global__ void reduce_sum_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    // Use a larger block size for better occupancy and CUB efficiency
    typedef cub::BlockReduce<int, 256> BlockReduce;
    __shared__ typename BlockReduce::Storage storage;
    BlockReduce block_reduce;

    int sum = 0;
    int tid = threadIdx.x;
    int grid_stride = blockDim.x * gridDim.x;

    // Vectorized loading to increase memory bandwidth utilization
    // Process 4 elements per thread per iteration
    const int4* input_v4 = reinterpret_cast<const int4*>(input);
    int n_v4 = n / 4;
    
    for (int i = tid * 4; i < n_v4 * grid_stride; i += grid_stride * 4) {
        int4 val = input_v4[i];
        sum += val.x + val.y + val.z + val.w;
    }

    // Handle remaining elements if n is not a multiple of 4 * grid_stride
    // This is a simplified approach; for absolute correctness with any n:
    int start_idx = (blockIdx.x * blockDim.x + tid) * 4 + (n_v4 * 4 - (n_v4 * 4)); // logic simplified
    // Correcting the loop for general n:
    // Reset sum and use a standard grid-stride loop for simplicity and correctness
}

// Refined kernel for correctness and performance
__global__ void reduce_sum_optimized(const int* __restrict__ input, int* __restrict__ output, int n) {
    typedef cub::BlockReduce<int, 256> BlockReduce;
    __shared__ typename BlockReduce::Storage storage;
    BlockReduce block_reduce;

    int local_sum = 0;
    int tid = threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    for (int i = tid; i < n; i += stride) {
        local_sum += input[i];
    }

    int block_sum = block_reduce.Sum(local_sum);

    if (tid == 0) {
        atomicAdd(output, block_sum);
    }
}

} // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    const int block_size = 256;
    // Calculate grid size to saturate the GPU
    int grid_size = (n + block_size - 1) / block_size;
    if (grid_size > 1024) grid_size = 1024; // Cap grid size to avoid excessive overhead
    if (grid_size == 0) grid_size = 1;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, sizeof(int), 0);
        reduce_sum_optimized<<<grid_size, block_size>>>(input, output, n);
    }
    cudaDeviceSynchronize();
}