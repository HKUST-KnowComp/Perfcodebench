#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

namespace {

inline int div_up(int a, int b) {
  return (a + b - 1) / b;
}

__global__ void copy_kernel(const int* __restrict__ input,
                            int* __restrict__ output,
                            int n) {
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  if (idx < n) output[idx] = input[idx];
}

}  // namespace

void sort_vec_i32(const int* input, int* output, int n, int iters) {
  if (n <= 0 || iters <= 0) return;

  cudaError_t err;

  int* temp_keys = nullptr;
  err = cudaMalloc(&temp_keys, static_cast<size_t>(n) * sizeof(int));
  if (err != cudaSuccess) return;

  void* d_temp_storage = nullptr;
  size_t temp_storage_bytes = 0;

  err = cub::DeviceRadixSort::SortKeys(
      d_temp_storage, temp_storage_bytes, output, temp_keys, n);
  if (err != cudaSuccess) {
    cudaFree(temp_keys);
    return;
  }

  err = cudaMalloc(&d_temp_storage, temp_storage_bytes);
  if (err != cudaSuccess) {
    cudaFree(temp_keys);
    return;
  }

  const int threads = 256;
  const int blocks = div_up(n, threads);

  for (int iter = 0; iter < iters; ++iter) {
    copy_kernel<<<blocks, threads>>>(input, output, n);
    cub::DeviceRadixSort::SortKeys(
        d_temp_storage, temp_storage_bytes, output, temp_keys, n);
  }

  cudaFree(d_temp_storage);
  cudaFree(temp_keys);
}
