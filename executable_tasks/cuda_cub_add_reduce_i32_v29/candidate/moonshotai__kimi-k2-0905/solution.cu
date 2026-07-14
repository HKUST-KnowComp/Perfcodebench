#include "interface.h"
#include <cub/cub.cuh>

void add_reduce_i32(const int* input, int* output, int n, int iters) {
  // Determine temporary storage size once (first call with nullptr).
  size_t temp_storage_bytes = 0;
  cub::DeviceReduce::Sum(nullptr, temp_storage_bytes, input, output, n);

  // Allocate temporary storage once.
  void* d_temp_storage = nullptr;
  cudaMalloc(&d_temp_storage, temp_storage_bytes);

  // Run reduction repeatedly.
  for (int i = 0; i < iters; ++i) {
    cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
  }

  cudaFree(d_temp_storage);
}