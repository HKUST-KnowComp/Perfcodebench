#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  if (n <= 0 || iters <= 0) return;

  // Allocate CUB temporary storage for reduction
  void* d_temp = nullptr;
  size_t temp_bytes = 0;
  cub::DeviceReduce::Sum(d_temp, temp_bytes, input, output, n);
  cudaMalloc(&d_temp, temp_bytes);

  // Run reduction for all iterations
  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, sizeof(int));
    cub::DeviceReduce::Sum(d_temp, temp_bytes, input, output, n);
  }

  // Clean up temporary storage
  cudaFree(d_temp);
}