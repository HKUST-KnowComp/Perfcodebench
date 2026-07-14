#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
  if (n <= 0 || iters <= 0) return;
  
  // Allocate temporary storage required for CUB's parallel scan
  void* d_temp_storage = nullptr;
  size_t temp_storage_bytes = 0;
  // First call to query required temporary storage size
  cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_storage_bytes, input, output, n);
  cudaMalloc(&d_temp_storage, temp_storage_bytes);
  
  // Run scan for all requested iterations
  for (int iter = 0; iter < iters; ++iter) {
    cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_storage_bytes, input, output, n);
  }
  
  // Cleanup device memory
  cudaFree(d_temp_storage);
}