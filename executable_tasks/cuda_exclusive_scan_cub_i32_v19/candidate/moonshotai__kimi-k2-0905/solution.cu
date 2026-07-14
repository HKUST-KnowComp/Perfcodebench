#include "interface.h"
#include <cub/cub.cuh>
#include <cuda_runtime.h>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
  size_t temp_bytes = 0;
  cub::DeviceScan::ExclusiveSum(nullptr, temp_bytes, input, output, n);
  void* d_temp = nullptr;
  cudaMalloc(&d_temp, temp_bytes);
  for (int i = 0; i < iters; ++i) {
    cub::DeviceScan::ExclusiveSum(d_temp, temp_bytes, input, output, n);
  }
  cudaFree(d_temp);
}