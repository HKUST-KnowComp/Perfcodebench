#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

namespace {

struct TempStorageCache {
  void* ptr;
  size_t bytes;
  TempStorageCache() : ptr(nullptr), bytes(0) {}
  ~TempStorageCache() {
    if (ptr) cudaFree(ptr);
  }
  void ensure(size_t required) {
    if (required <= bytes) return;
    if (ptr) cudaFree(ptr);
    cudaMalloc(&ptr, required);
    bytes = required;
  }
};

TempStorageCache& storage_cache() {
  static TempStorageCache cache;
  return cache;
}

}  // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
  if (iters <= 0) return;

  if (n <= 0) {
    for (int iter = 0; iter < iters; ++iter) {
      cudaMemset(output, 0, sizeof(int));
    }
    return;
  }

  size_t temp_storage_bytes = 0;
  cub::DeviceReduce::Sum(nullptr, temp_storage_bytes, input, output, n);
  TempStorageCache& cache = storage_cache();
  cache.ensure(temp_storage_bytes);

  for (int iter = 0; iter < iters; ++iter) {
    cub::DeviceReduce::Sum(cache.ptr, temp_storage_bytes, input, output, n);
  }
}
