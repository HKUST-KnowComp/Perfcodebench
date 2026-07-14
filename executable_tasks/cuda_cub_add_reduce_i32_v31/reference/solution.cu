#include "interface.h"
#include <cuda_runtime.h>
#include <cub/device/device_reduce.cuh>
void add_reduce_i32(const int* input, int* output, int n, int iters) {
  void* temp_storage = nullptr; std::size_t temp_bytes = 0; cub::DeviceReduce::Sum(temp_storage, temp_bytes, input, output, n); cudaMalloc(&temp_storage, temp_bytes); for (int iter = 0; iter < iters; ++iter) cub::DeviceReduce::Sum(temp_storage, temp_bytes, input, output, n); cudaFree(temp_storage);
}
