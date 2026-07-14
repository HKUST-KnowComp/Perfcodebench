#include "interface.h"
#include <cub/cub.cuh>
#include <cuda_runtime.h>

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  void* d_temp = nullptr;
  size_t temp_bytes = 0;
  // Query CUB's required temporary storage size for sum reduction
  cub::DeviceReduce::Sum(d_temp, temp_bytes, input, output, n);
  // Allocate temporary storage once before iteration loop
  cudaMalloc(&d_temp, temp_bytes);
  // Run optimized reduction for all iterations
  for (int iter = 0; iter < iters; ++iter) {
    cub::DeviceReduce::Sum(d_temp, temp_bytes, input, output, n);
  }
  // Clean up allocated temporary storage
  cudaFree(d_temp);
}