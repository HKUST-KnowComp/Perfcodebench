#include "interface.h"

#include <cuda_runtime.h>
#include <cub/cub.cuh>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
  if (n <= 0 || iters <= 0) return;

  // Determine temporary storage requirements
  std::size_t temp_storage_bytes = 0;
  cub::DeviceScan::ExclusiveSum(
      nullptr, temp_storage_bytes, input, output, n);

  // Allocate temporary storage
  void* d_temp_storage = nullptr;
  cudaMalloc(&d_temp_storage, temp_storage_bytes);

  // Perform multiple exclusive scans
  for (int i = 0; i < iters; ++i) {
    cub::DeviceScan::ExclusiveSum(
        d_temp_storage, temp_storage_bytes, input, output, n);
  }

  // Wait for all launched work to complete before freeing resources
  cudaDeviceSynchronize();
  cudaFree(d_temp_storage);
}
