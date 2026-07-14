#include "interface.h"

#include <cuda_runtime.h>
#include <cub/device/device_scan.cuh>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
  if (n <= 0 || iters <= 0) {
    return;
  }

  // Determine temporary storage size for CUB DeviceScan
  size_t temp_bytes = 0;
  cub::DeviceScan::ExclusiveSum(nullptr, temp_bytes, input, output, n);

  void* d_temp = nullptr;
  if (temp_bytes > 0) {
    cudaMalloc(&d_temp, temp_bytes);
  }

  for (int iter = 0; iter < iters; ++iter) {
    cub::DeviceScan::ExclusiveSum(d_temp, temp_bytes, input, output, n);
  }

  if (d_temp) {
    cudaFree(d_temp);
  }
}
