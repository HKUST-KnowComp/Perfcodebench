#include "interface.h"

#include <cub/cub.cuh>
#include <cuda_runtime.h>

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  // Ask CUB for the temporary storage size once; reuse for every iteration.
  size_t temp_storage_bytes = 0;
  cub::DeviceReduce::Sum(nullptr, temp_storage_bytes, input, output, n);

  // Allocate persistent temporary storage.
  void* d_temp_storage = nullptr;
  cudaMalloc(&d_temp_storage, temp_storage_bytes);

  for (int iter = 0; iter < iters; ++iter) {
    cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes, input, output, n);
  }

  cudaFree(d_temp_storage);
}