#include "interface.h"

#include <cuda_runtime.h>
#include <cub/cub.cuh>

namespace {

struct TempStorageCache {
  void* ptr = nullptr;
  size_t bytes = 0;
  int device = -1;

  ~TempStorageCache() {
    if (ptr) {
      cudaFree(ptr);
    }
  }

  void ensure(size_t required_bytes) {
    int current_device = 0;
    cudaGetDevice(&current_device);

    if (ptr && (device != current_device || bytes < required_bytes)) {
      cudaFree(ptr);
      ptr = nullptr;
      bytes = 0;
      device = -1;
    }

    if (!ptr) {
      cudaMalloc(&ptr, required_bytes);
      bytes = required_bytes;
      device = current_device;
    }
  }
};

TempStorageCache& storage_cache() {
  static TempStorageCache cache;
  return cache;
}

}  // namespace

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
  if (iters <= 0) return;

  size_t temp_storage_bytes = 0;
  cub::DeviceScan::ExclusiveSum(nullptr, temp_storage_bytes, input, output, n);

  if (temp_storage_bytes > 0) {
    TempStorageCache& cache = storage_cache();
    cache.ensure(temp_storage_bytes);
    for (int iter = 0; iter < iters; ++iter) {
      cub::DeviceScan::ExclusiveSum(cache.ptr, temp_storage_bytes, input, output, n);
    }
  } else {
    for (int iter = 0; iter < iters; ++iter) {
      cub::DeviceScan::ExclusiveSum(nullptr, temp_storage_bytes, input, output, n);
    }
  }
}
