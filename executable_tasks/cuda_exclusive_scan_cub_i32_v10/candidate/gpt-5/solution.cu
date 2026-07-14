#include "interface.h"

#include <cuda_runtime.h>
#include <cub/cub.cuh>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
  if (iters <= 0) return;
  if (n <= 0) return;

  void* d_temp_storage = nullptr;
  size_t temp_storage_bytes = 0;

  // Query temp storage size
  cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_storage_bytes, input, output, n);

  // Allocate temp storage once and reuse across iterations
  cudaMalloc(&d_temp_storage, temp_storage_bytes);

  for (int iter = 0; iter < iters; ++iter) {
    cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_storage_bytes, input, output, n);
  }

  // Free temp storage
  cudaFree(d_temp_storage);
}
