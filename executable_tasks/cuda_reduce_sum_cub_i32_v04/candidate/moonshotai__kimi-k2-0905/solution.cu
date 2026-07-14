#include "interface.h"

#include <cub/cub.cuh>
#include <cuda_runtime.h>

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  // Allocate temporary storage once and reuse across iterations
  static void* d_temp_storage = nullptr;
  static size_t temp_storage_bytes = 0;

  // Query required buffer size (does nothing on subsequent calls)
  cub::DeviceReduce::Sum(nullptr, temp_storage_bytes, input, output, n);
  if (d_temp_storage == nullptr) {
    cudaMalloc(&d_temp_storage, temp_storage_bytes);
  }

  for (int iter = 0; iter < iters; ++iter) {
    cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
  }
  cudaDeviceSynchronize();  // ensure all reductions finish
}