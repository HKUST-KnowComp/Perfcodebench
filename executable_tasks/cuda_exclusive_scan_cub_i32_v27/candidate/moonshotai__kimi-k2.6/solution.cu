#include "interface.h"

#include <cuda_runtime.h>
#include <cub/cub.cuh>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
  if (n <= 0 || iters <= 0) {
    return;
  }

  // Query temporary device storage requirements
  size_t temp_storage_bytes = 0;
  cub::DeviceScan::ExclusiveSum(nullptr, temp_storage_bytes, input, output, n);

  // Allocate temporary storage once
  void* d_temp_storage = nullptr;
  cudaMalloc(&d_temp_storage, temp_storage_bytes);

  // Run the optimized exclusive scan for the requested number of iterations
  for (int iter = 0; iter < iters; ++iter) {
    cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_storage_bytes, input, output, n);
  }

  // Release temporary storage
  cudaFree(d_temp_storage);
}
