#include "interface.h"

#include <cub/device/device_scan.cuh>
#include <cuda_runtime.h>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
  if (n <= 0 || iters <= 0) {
    return;
  }

  size_t temp_storage_bytes = 0;
  cub::DeviceScan::ExclusiveSum(nullptr, temp_storage_bytes, input, output, n);

  void* temp_storage = nullptr;
  if (temp_storage_bytes > 0) {
    cudaMalloc(&temp_storage, temp_storage_bytes);
  }

  for (int i = 0; i < iters; ++i) {
    cub::DeviceScan::ExclusiveSum(temp_storage, temp_storage_bytes, input, output, n);
  }

  if (temp_storage) {
    cudaFree(temp_storage);
  }
}
