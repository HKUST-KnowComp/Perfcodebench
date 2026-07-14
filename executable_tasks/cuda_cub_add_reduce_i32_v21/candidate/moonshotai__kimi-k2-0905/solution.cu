#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

namespace {

__global__ void reduce_kernel(const int* __restrict__ input,
                              int* __restrict__ output,
                              int n) {
    using BlockReduce = cub::BlockReduce<int, 256>;   // 256 threads per block
    __shared__ typename BlockReduce::TempStorage temp;

    int sum = 0;
    for (int idx = blockIdx.x * blockDim.x + threadIdx.x;
         idx < n;
         idx += blockDim.x * gridDim.x) {
        sum += input[idx];
    }
    sum = BlockReduce(temp).Sum(sum);

    if (threadIdx.x == 0) {
        atomicAdd(output, sum);
    }
}

}  // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    const int block_size = 256;
    const int num_blocks = (n + block_size - 1) / block_size;
    const int max_blocks = 4 * 1024;  // empirical sweet spot (≈SM count * 4)
    const int grid_size  = min(num_blocks, max_blocks);

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<<<grid_size, block_size>>>(input, output, n);
    }
}