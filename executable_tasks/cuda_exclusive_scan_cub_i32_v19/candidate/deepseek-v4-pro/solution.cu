#include "interface.h"

#include <cuda_runtime.h>
#include <cub/cub.cuh>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
  if (n <= 0) return;

  // Determine required temporary storage size
  size_t temp_bytes = 0;
  cub::DeviceScan::ExclusiveSum(nullptr, temp_bytes, input, output, n);

  // Allocate temporary storage once
  void* d_temp_storage = nullptr;
  cudaMalloc(&d_temp_storage, temp_bytes);

  // Perform the exclusive sum 'iters' times
  for (int iter = 0; iter < iters; ++iter) {
    cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_bytes, input, output, n);
  }

  // Free temporary storage (will be scheduled after all submitted work)
  cudaFree(d_temp_storage);
}
