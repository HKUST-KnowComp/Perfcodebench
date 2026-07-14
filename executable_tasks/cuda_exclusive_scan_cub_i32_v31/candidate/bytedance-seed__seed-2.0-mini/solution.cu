#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
  static int* d_temp_storage = nullptr;
  static size_t temp_storage_size = 0;

  if (n > 0) {
    size_t required_bytes = 0;
    cub::DeviceScan::ExclusiveSum(nullptr, required_bytes, input, output, n);

    if (required_bytes > temp_storage_size) {
      cudaFree(d_temp_storage);
      d_temp_storage = nullptr;
      if (required_bytes > 0) {
        cudaMalloc(&d_temp_storage, required_bytes);
      }
      temp_storage_size = required_bytes;
    }
  }

  for (int iter = 0; iter < iters; ++iter) {
    if (n > 0) {
      cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_storage_size, input, output, n, 0);
    }
  }
}