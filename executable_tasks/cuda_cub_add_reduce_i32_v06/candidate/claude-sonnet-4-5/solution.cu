#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

namespace {
  void* d_temp_storage = nullptr;
  size_t temp_storage_bytes = 0;
  bool initialized = false;
}

void add_reduce_i32(const int* input, int* output, int n, int iters) {
  if (!initialized) {
    cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
    cudaMalloc(&d_temp_storage, temp_storage_bytes);
    initialized = true;
  }
  
  for (int iter = 0; iter < iters; ++iter) {
    cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
  }
  cudaDeviceSynchronize();
}