#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
  size_t temp_storage_bytes;
  cub::DeviceScan::ExclusiveSum(nullptr, temp_storage_bytes, input, output, n);
  int* d_temp;
  cudaMalloc(&d_temp, temp_storage_bytes);
  for (int iter = 0; iter < iters; ++iter) {
    cub::DeviceScan::ExclusiveSum(d_temp, temp_storage_bytes, input, output, n);
  }
  cudaFree(d_temp);
}