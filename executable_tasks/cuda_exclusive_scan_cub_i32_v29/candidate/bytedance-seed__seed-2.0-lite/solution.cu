#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
  if (n <= 0 || iters <= 0) return;

  // Query required temporary storage size for CUB's scan
  void* d_temp = nullptr;
  size_t temp_storage_bytes = 0;
  cub::DeviceScan::ExclusiveSum(d_temp, temp_storage_bytes, input, output, n);

  // Allocate temporary storage once to reuse across all iterations
  cudaMalloc(&d_temp, temp_storage_bytes);

  // Launch optimized parallel scan for each iteration
  for (int iter = 0; iter < iters; ++iter) {
    cub::DeviceScan::ExclusiveSum(d_temp, temp_storage_bytes, input, output, n);
  }

  // Cleanup temporary resources
  cudaFree(d_temp);
}