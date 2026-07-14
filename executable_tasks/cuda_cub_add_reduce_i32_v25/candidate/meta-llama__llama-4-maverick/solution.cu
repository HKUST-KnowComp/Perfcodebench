#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

namespace {
void cub_reduce(const int* input, int* output, int n) {
  int* temp_storage = nullptr;
  size_t temp_storage_size = 0;
  cub::DeviceReduce::Sum(temp_storage, temp_storage_size, input, output, n);
  cudaMalloc(&temp_storage, temp_storage_size);
  cub::DeviceReduce::Sum(temp_storage, temp_storage_size, input, output, n);
  cudaFree(temp_storage);
}
}  // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
  for (int iter = 0; iter < iters; ++iter) {
    cub_reduce(input, output, n);
  }
}