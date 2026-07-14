#include "interface.h"
#include <cub/cub.cuh>
#include <cuda_runtime.h>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
  if (n <= 0 || iters <= 0) return;

  // Determine temporary device storage requirements
  void* d_temp_storage = nullptr;
  size_t temp_storage_bytes = 0;
  cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_storage_bytes, input, output, n);
  // Allocate the required temporary storage
  cudaMalloc(&d_temp_storage, temp_storage_bytes);

  // Run the parallel exclusive scan the requested number of times
  for (int iter = 0; iter < iters; ++iter) {
    cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_storage_bytes, input, output, n);
  }

  // Free temporary storage
  cudaFree(d_temp_storage);
}