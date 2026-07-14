#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  static void* d_temp_storage = nullptr;
  static size_t temp_storage_bytes = 0;
  static int cached_n = -1;

  if (n != cached_n) {
    if (d_temp_storage != nullptr) {
      cudaFree(d_temp_storage);
      d_temp_storage = nullptr;
    }
    cub::DeviceReduce::Sum(nullptr, temp_storage_bytes, static_cast<const int*>(nullptr), static_cast<int*>(nullptr), n, cudaStreamDefault);
    cudaMalloc(&d_temp_storage, temp_storage_bytes);
    cached_n = n;
  }

  for (int iter = 0; iter < iters; ++iter) {
    cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n, cudaStreamDefault);
  }
}