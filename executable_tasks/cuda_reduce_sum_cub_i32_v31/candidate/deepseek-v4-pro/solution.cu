#include "interface.h"

#include <cuda_runtime.h>
#include <cub/cub.cuh>

namespace {

// No custom kernel; use CUB for optimal reduction.

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  void* d_temp_storage = nullptr;
  size_t temp_storage_bytes = 0;
  // Determine required temporary storage size.
  cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
  if (temp_storage_bytes > 0) {
    cudaMalloc(&d_temp_storage, temp_storage_bytes);
  }

  // Perform the reduction iters times, reusing temporary storage.
  for (int iter = 0; iter < iters; ++iter) {
    cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
  }

  if (d_temp_storage) {
    cudaFree(d_temp_storage);
  }
}