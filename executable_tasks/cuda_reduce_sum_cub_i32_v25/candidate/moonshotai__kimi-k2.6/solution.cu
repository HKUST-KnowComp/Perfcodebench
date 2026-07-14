#include "interface.h"

#include <cuda_runtime.h>
#include <cub/cub.cuh>

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  if (n <= 0 || iters <= 0) {
    return;
  }

  void* d_temp_storage = nullptr;
  size_t temp_storage_bytes = 0;

  // Query temporary storage requirements
  cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);

  // Allocate temporary storage if required
  if (temp_storage_bytes > 0) {
    cudaMalloc(&d_temp_storage, temp_storage_bytes);
  }

  // Run the reduction for the requested number of iterations
  for (int i = 0; i < iters; ++i) {
    cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
  }

  // Free temporary storage
  if (d_temp_storage) {
    cudaFree(d_temp_storage);
  }
}
