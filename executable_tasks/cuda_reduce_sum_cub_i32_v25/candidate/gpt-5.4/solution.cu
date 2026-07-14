#include "interface.h"

#include <cuda_runtime.h>
#include <cub/cub.cuh>

namespace {

struct CubReduceCache {
  void* temp_storage = nullptr;
  size_t temp_storage_bytes = 0;

  ~CubReduceCache() {
    if (temp_storage) {
      cudaFree(temp_storage);
    }
  }
};

static CubReduceCache& get_cache() {
  static CubReduceCache cache;
  return cache;
}

static inline void ensure_temp_storage(size_t required_bytes) {
  CubReduceCache& cache = get_cache();
  if (cache.temp_storage_bytes >= required_bytes) return;

  if (cache.temp_storage) {
    cudaFree(cache.temp_storage);
    cache.temp_storage = nullptr;
    cache.temp_storage_bytes = 0;
  }

  if (required_bytes > 0) {
    cudaMalloc(&cache.temp_storage, required_bytes);
    cache.temp_storage_bytes = required_bytes;
  }
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  if (iters <= 0) return;

  if (n <= 0) {
    for (int iter = 0; iter < iters; ++iter) {
      cudaMemset(output, 0, sizeof(int));
    }
    return;
  }

  size_t temp_storage_bytes = 0;
  cub::DeviceReduce::Sum(nullptr, temp_storage_bytes, input, output, n);
  ensure_temp_storage(temp_storage_bytes);

  void* temp_storage = get_cache().temp_storage;
  for (int iter = 0; iter < iters; ++iter) {
    cub::DeviceReduce::Sum(temp_storage, temp_storage_bytes, input, output, n);
  }
}
