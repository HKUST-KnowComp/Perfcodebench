#include "interface.h"
#include <cub/cub.cuh>
#include <cuda_runtime.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
  void* d_temp_storage = nullptr;
  size_t temp_storage_bytes = 0;
  
  cub::DeviceRadixSort::SortKeys(d_temp_storage, temp_storage_bytes, input, output, n);
  cudaMalloc(&d_temp_storage, temp_storage_bytes);
  
  for (int iter = 0; iter < iters; ++iter) {
    cub::DeviceRadixSort::SortKeys(d_temp_storage, temp_storage_bytes, input, output, n);
  }
  
  cudaFree(d_temp_storage);
}