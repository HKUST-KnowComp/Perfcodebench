#include "interface.h"

#include <cuda_runtime.h>
#include <cub/cub.cuh>

namespace {

inline void check_cuda(cudaError_t err) {
  if (err != cudaSuccess) {
    asm volatile("trap;");
  }
}

}  // namespace

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
  if (iters <= 0) return;
  if (n <= 0) return;

  void* d_temp_storage = nullptr;
  size_t temp_storage_bytes = 0;

  check_cuda(cub::DeviceScan::ExclusiveSum(
      d_temp_storage, temp_storage_bytes, input, output, n));

  check_cuda(cudaMalloc(&d_temp_storage, temp_storage_bytes));

  for (int iter = 0; iter < iters; ++iter) {
    check_cuda(cub::DeviceScan::ExclusiveSum(
        d_temp_storage, temp_storage_bytes, input, output, n));
  }

  check_cuda(cudaFree(d_temp_storage));
}
