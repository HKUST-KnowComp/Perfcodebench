#include "interface.h"

#include <cuda_runtime.h>
#include <cub/cub.cuh>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
  // Determine temporary storage size once (CUB does it internally)
  size_t temp_bytes = 0;
  cub::DeviceScan::ExclusiveSum(nullptr, temp_bytes, input, output, n);

  // Allocate temporary storage on the device
  void* d_temp = nullptr;
  cudaMalloc(&d_temp, temp_bytes);

  for (int iter = 0; iter < iters; ++iter) {
    cub::DeviceScan::ExclusiveSum(d_temp, temp_bytes, input, output, n);
  }

  cudaFree(d_temp);
}