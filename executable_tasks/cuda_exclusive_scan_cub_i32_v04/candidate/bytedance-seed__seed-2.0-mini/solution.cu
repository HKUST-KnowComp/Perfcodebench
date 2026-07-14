#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
  if (n <= 0 || iters <= 0) {
    return;
  }

  // Query required temporary storage size once per function invocation
  size_t temp_storage_bytes = 0;
  cub::DeviceScan::ExclusiveScan(nullptr, temp_storage_bytes, input, output, cub::Sum(), n);

  // Allocate permanent temporary storage for all iterations
  void* d_temp_storage = nullptr;
  const cudaError_t alloc_err = cudaMalloc(&d_temp_storage, temp_storage_bytes);
  if (alloc_err != cudaSuccess) {
    return;
  }

  // Execute exclusive scan for each requested iteration
  for (int iter = 0; iter < iters; ++iter) {
    cub::DeviceScan::ExclusiveScan(d_temp_storage, temp_storage_bytes, input, output, cub::Sum(), n);
  }

  // Clean up temporary storage
  cudaFree(d_temp_storage);
}