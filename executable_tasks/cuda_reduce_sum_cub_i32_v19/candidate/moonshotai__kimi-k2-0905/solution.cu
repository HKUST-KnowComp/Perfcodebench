#include "interface.h"
#include <cub/cub.cuh>
#include <cuda_runtime.h>
#include <cstdint>

namespace {

// Device buffer for the reduction result (persistent across iterations)
__device__ int g_device_sum;

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  // Temporary storage required by CUB
  void* d_temp_storage = nullptr;
  size_t temp_storage_bytes = 0;

  // Query required temporary storage size
  cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes,
                         input, &g_device_sum, n);
  // Allocate temporary storage
  cudaMalloc(&d_temp_storage, temp_storage_bytes);

  // Perform reduction `iters` times, re-using the same temp storage
  for (int i = 0; i < iters; ++i) {
    cub::DeviceReduce::Sum(d_temp_storage, temp_storage_bytes,
                           input, &g_device_sum, n);
  }

  // Copy final result back to host-visible output pointer
  cudaMemcpy(output, &g_device_sum, sizeof(int), cudaMemcpyDeviceToHost);

  // Clean up temporary storage
  cudaFree(d_temp_storage);
}