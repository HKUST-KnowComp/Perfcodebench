#include "interface.h"

#include <cuda_runtime.h>
#include <cub/cub.cuh>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
  if (n <= 0 || iters <= 0) {
    return;
  }

  // Cache CUB temporary storage across calls to minimize allocation overhead
  static void* d_temp_storage = nullptr;
  static size_t temp_storage_bytes = 0;

  // Determine required temporary storage size for given problem size
  size_t required_bytes = 0;
  cub::DeviceScan::ExclusiveSum(nullptr, required_bytes, input, output, n);

  // Ensure sufficient temporary storage is allocated
  if (d_temp_storage == nullptr || required_bytes > temp_storage_bytes) {
    if (d_temp_storage) {
      cudaFree(d_temp_storage);
      d_temp_storage = nullptr;
      temp_storage_bytes = 0;
    }
    cudaMalloc(&d_temp_storage, required_bytes);
    temp_storage_bytes = required_bytes;
  }

  // Perform the exclusive scan the requested number of iterations
  for (int iter = 0; iter < iters; ++iter) {
    cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_storage_bytes, input, output, n);
  }
}
