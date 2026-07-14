#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  // Create CUB reduction object
  cub::DeviceReduce::Sum(nullptr, size_t{}, input, output, n);
  size_t temp_storage_bytes = size_t{};
  void* temp_storage = nullptr;
  cudaMalloc(&temp_storage, temp_storage_bytes);
  cub::DeviceReduce::Sum(temp_storage, temp_storage_bytes, input, output, n);
  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, sizeof(int));
    cub::DeviceReduce::Sum(temp_storage, temp_storage_bytes, input, output, n);
  }
  cudaFree(temp_storage);
}