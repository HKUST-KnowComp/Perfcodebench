#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void atomic_reduce_kernel(const int* input, int* output, int n) {
  for (int idx = blockIdx.x * blockDim.x + threadIdx.x; idx < n; idx += blockDim.x * gridDim.x) {
    atomicAdd(output, input[idx]);
  }
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  constexpr int kBlock = 256;
  const int grid = 1024;
  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, sizeof(int));
    atomic_reduce_kernel<<<grid, kBlock>>>(input, output, n);
  }
}
