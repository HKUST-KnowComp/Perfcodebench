#include <cub/cub.cuh>
#include "interface.h"

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  void* d_temp_storage = nullptr;
  size_t temp_storage_bytes = 0;

  // Query required temporary storage size
  cub::DeviceReduce::Sum(nullptr, temp_storage_bytes, input, output, n);

  // Allocate temporary storage once
  cudaMalloc(&d_temp_storage, temp_storage_bytes);

  for (int iter = 0; iter < iters; ++iter) {
    // Perform optimized reduction
    cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
  }

  cudaFree(d_temp_storage);
}