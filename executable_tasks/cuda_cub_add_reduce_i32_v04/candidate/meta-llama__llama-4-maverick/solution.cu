#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void add_reduce_i32(const int* input, int* output, int n, int iters) {
  // Create CUB's reduce object
  cub::DeviceReduce::Sum(nullptr, size_t(0), input, output, n);
  size_t temp_storage_bytes = size_t(0);
  cub::DeviceReduce::Sum(&temp_storage_bytes, size_t(0), input, output, n);
  void* temp_storage = nullptr;
  cudaMalloc(&temp_storage, temp_storage_bytes);
  for (int iter = 0; iter < iters; ++iter) {
    int init = 0;
    cub::DeviceReduce::Sum(temp_storage, temp_storage_bytes, input, output, n);
  }
  cudaFree(temp_storage);
}