#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

namespace {
__global__ void add_reduce_kernel(const int* input, int* output, int n) {
  int tid = blockIdx.x * blockDim.x + threadIdx.x;
  if (tid >= n) return;
  *output = *input;
}

__global__ void add_reduce_finalize(int* output) {
  // For a single block with one active thread
  *output = blockReduce(output[0], cub::Sum());
}
}  // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
  int* d_temp_storage = nullptr;
  size_t temp_storage_bytes = 0;
  cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
  cudaMalloc((void**)&d_temp_storage, temp_storage_bytes);

  for (int iter = 0; iter < iters; ++iter) {
    cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
  }

  cudaFree(d_temp_storage);
}
