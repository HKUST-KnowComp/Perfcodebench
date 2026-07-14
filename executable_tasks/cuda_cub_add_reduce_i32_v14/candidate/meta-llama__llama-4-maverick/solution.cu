#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void add_reduce_i32(const int* input, int* output, int n, int iters) {
  // Declare temporary storage and sum
  void* d_temp_storage = nullptr;
  size_t temp_storage_bytes = 0;
  int sum;
  cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
  // Allocate temporary storage
  cudaMalloc(&d_temp_storage, temp_storage_bytes);
  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, sizeof(int));
    cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
  }
  cudaFree(d_temp_storage);
}