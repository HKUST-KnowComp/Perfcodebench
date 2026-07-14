#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
  // Determine required temporary storage size
  size_t temp_storage_bytes = 0;
  cub::DeviceScan::ExclusiveSum(nullptr, temp_storage_bytes, input, output, n);
  
  // Cache temporary buffer across calls to avoid repeated allocation
  static void* d_temp_storage = nullptr;
  static size_t allocated_bytes = 0;
  if (temp_storage_bytes > allocated_bytes) {
    if (d_temp_storage) cudaFree(d_temp_storage);
    cudaMalloc(&d_temp_storage, temp_storage_bytes);
    allocated_bytes = temp_storage_bytes;
  }
  
  // Perform the requested number of scans
  for (int iter = 0; iter < iters; ++iter) {
    cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_storage_bytes, input, output, n);
  }
}