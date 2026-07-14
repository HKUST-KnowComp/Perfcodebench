#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
  if (n <= 0 || iters <= 0) return;

  // Query required temporary storage size for CUB scan
  void* d_temp_storage = nullptr;
  size_t temp_storage_bytes = 0;
  cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_storage_bytes, input, output, 0, n);

  // Allocate temporary storage once, reused across all iterations
  cudaMalloc(&d_temp_storage, temp_storage_bytes);

  // Run scan for all requested iterations
  for (int iter = 0; iter < iters; ++iter) {
    cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_storage_bytes, input, output, 0, n);
  }

  // Cleanup allocated memory
  cudaFree(d_temp_storage);
}