#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  // Temporary storage for CUB reduction
  void* d_temp_storage = nullptr;
  size_t temp_storage_bytes = 0;

  // Get the size of temporary storage needed for CUB reduction
  cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
  cudaMalloc(&d_temp_storage, temp_storage_bytes);

  for (int iter = 0; iter < iters; ++iter) {
    // Reset output to 0 before each reduction iteration
    cudaMemset(output, 0, sizeof(int));
    // Perform reduction using CUB
    cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
  }

  // Clean up temporary storage
  cudaFree(d_temp_storage);
}