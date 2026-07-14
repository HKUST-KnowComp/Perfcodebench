#include "interface.h"

#include <cub/cub.cuh>
#include <cuda_runtime.h>

namespace {

__global__ void reduce_kernel(const int* input, int* output, int n) {
  typedef cub::BlockReduce<int, 256> BlockReduce;
  __shared__ typename BlockReduce::TempStorage temp_storage;

  int my_sum = 0;
  for (int idx = blockIdx.x * blockDim.x + threadIdx.x; idx < n;
       idx += blockDim.x * gridDim.x) {
    my_sum += input[idx];
  }

  int block_sum = BlockReduce(temp_storage).Sum(my_sum);

  if (threadIdx.x == 0) {
    atomicAdd(output, block_sum);
  }
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  constexpr int kBlock = 256;
  // Adapt grid size: use enough blocks to cover all elements, but cap at 1024
  // to limit atomic contention and avoid launching many empty blocks.
  int grid = (n + kBlock - 1) / kBlock;
  if (grid > 1024) grid = 1024;

  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, sizeof(int));
    reduce_kernel<<<grid, kBlock>>>(input, output, n);
  }
}