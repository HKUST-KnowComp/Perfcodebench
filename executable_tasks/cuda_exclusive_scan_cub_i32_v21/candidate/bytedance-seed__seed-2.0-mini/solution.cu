#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

namespace {

__global__ void sequential_scan_kernel(const int* input, int* output, int n) {
  if (blockIdx.x != 0 || threadIdx.x != 0) {
    return;
  }
  int running = 0;
  for (int i = 0; i < n; ++i) {
    output[i] = running;
    running += input[i];
  }
}

}  // namespace

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
  // Early exit if no work to perform
  if (n <= 0 || iters <= 0) {
    return;
  }

  // Calculate required temporary storage size once
  size_t temp_storage_bytes = 0;
  cub::DeviceScan::ExclusiveSum(nullptr, temp_storage_bytes, input, output, n);

  void* d_temp_storage = nullptr;
  bool use_cub_optimized = true;
  
  // Attempt to allocate temporary device storage
  if (temp_storage_bytes > 0) {
    const cudaError_t alloc_err = cudaMalloc(&d_temp_storage, temp_storage_bytes);
    if (alloc_err != cudaSuccess) {
      use_cub_optimized = false;
    }
  }

  if (use_cub_optimized) {
    // Run optimized parallel exclusive scan for each iteration
    for (int iter = 0; iter < iters; ++iter) {
      cub::DeviceScan::ExclusiveSum(d_temp_storage, temp_storage_bytes, input, output, n);
    }
  } else {
    // Fall back to original single-threaded kernel if allocation fails
    for (int iter = 0; iter < iters; ++iter) {
      sequential_scan_kernel<<<1, 1>>>(input, output, n);
    }
  }

  // Clean up temporary storage
  if (d_temp_storage != nullptr) {
    cudaFree(d_temp_storage);
  }
}