#include "interface.h"

#include <cuda_runtime.h>
#include <cub/cub.cuh>

namespace {
// Cached temporary storage for CUB to avoid alloc/free overhead across calls
struct CubTempStorage {
  void* ptr = nullptr;
  size_t bytes = 0;
} g_temp;
}

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
  if (n <= 0 || iters <= 0) {
    return;
  }

  // Query required temporary storage size for this problem size
  size_t required_bytes = 0;
  cub::DeviceScan::ExclusiveSum(nullptr, required_bytes, input, output, n);

  // Ensure we have enough temporary storage allocated
  if (required_bytes > g_temp.bytes) {
    if (g_temp.ptr) {
      // Free previous allocation; since we don't synchronize here, only reallocate
      // when strictly necessary to minimize risk of freeing memory still in use.
      cudaFree(g_temp.ptr);
      g_temp.ptr = nullptr;
      g_temp.bytes = 0;
    }
    cudaMalloc(&g_temp.ptr, required_bytes);
    g_temp.bytes = required_bytes;
  }

  // Perform the scan iters times
  for (int iter = 0; iter < iters; ++iter) {
    cub::DeviceScan::ExclusiveSum(g_temp.ptr, g_temp.bytes, input, output, n);
  }
}
