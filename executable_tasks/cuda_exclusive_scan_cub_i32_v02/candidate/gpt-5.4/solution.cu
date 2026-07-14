#include "interface.h"

#include <cuda_runtime.h>
#include <cub/cub.cuh>

namespace {

inline int ceil_div(int a, int b) {
  return (a + b - 1) / b;
}

}  // namespace

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
  if (iters <= 0) return;
  if (n <= 0) return;

  void* d_temp_storage = nullptr;
  size_t temp_storage_bytes = 0;

  cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_storage_bytes, input, output, n);

  cudaError_t err = cudaMalloc(&d_temp_storage, temp_storage_bytes);
  if (err != cudaSuccess) {
    return;
  }

  for (int iter = 0; iter < iters; ++iter) {
    cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_storage_bytes, input, output, n);
  }

  cudaFree(d_temp_storage);
}
