#include "interface.h"
#include <cuda_runtime.h>
#include <cub/device/device_reduce.cuh>
#include <cstdint>

namespace {

__global__ void cast_sum_kernel(int64_t* d_in, int* d_out) {
  *d_out = static_cast<int>(*d_in);
}

}  // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
  if (n <= 0 || iters <= 0) {
    if (iters > 0 && output != nullptr) {
      cudaMemset(output, 0, sizeof(int));
    }
    return;
  }

  int64_t* d_sum64 = nullptr;
  void* d_temp_storage = nullptr;
  size_t temp_storage_bytes = 0;

  const cudaError_t alloc_sum_err = cudaMalloc(&d_sum64, sizeof(int64_t));
  if (alloc_sum_err != cudaSuccess) return;

  cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, d_sum64, n);

  const cudaError_t alloc_temp_err = cudaMalloc(&d_temp_storage, temp_storage_bytes);
  if (alloc_temp_err != cudaSuccess) {
    cudaFree(d_sum64);
    return;
  }

  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, sizeof(int));
    cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, d_sum64, n);
    cast_sum_kernel<<<1, 1>>>(d_sum64, output);
  }

  cudaFree(d_sum64);
  cudaFree(d_temp_storage);
}