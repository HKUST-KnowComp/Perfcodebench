#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  void* d_temp_storage = nullptr;
  size_t temp_storage_bytes = 0;

  // Get the required temporary storage size
  cub::DeviceReduce::Sum(nullptr, temp_storage_bytes, input, output, n);

  // Allocate the temporary storage
  cudaMalloc(&d_temp_storage, temp_storage_bytes);

  for (int iter = 0; iter < iters; ++iter) {
    // Perform the reduction
    cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
  }

  // Free the temporary storage
  cudaFree(d_temp_storage);
}