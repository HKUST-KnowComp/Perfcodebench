#include "interface.h"

#include <cub/cub.cuh>
#include <cuda_runtime.h>
#include <cstdint>

namespace {

__device__ int dev_tmp;

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  // Allocate persistent device storage for CUB once
  static void* d_temp_storage = nullptr;
  static std::size_t temp_storage_bytes = 0;

  if (d_temp_storage == nullptr) {
    cub::DeviceReduce::Sum(nullptr, temp_storage_bytes, input, &dev_tmp, n);
    cudaMalloc(&d_temp_storage, temp_storage_bytes);
  }

  for (int iter = 0; iter < iters; ++iter) {
    cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes,
                           input, output, n);
  }
}
