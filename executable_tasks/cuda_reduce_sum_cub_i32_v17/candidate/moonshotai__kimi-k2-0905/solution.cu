#include "interface.h"

#include <cub/cub.cuh>
#include <cuda_runtime.h>

namespace {

struct SumInt32 {
  __device__ __forceinline__ int operator()(const int& a, const int& b) const {
    return a + b;
  }
};

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  // Allocate once and reuse for all iterations
  static void* d_temp_storage = nullptr;
  static size_t temp_storage_bytes = 0;

  // First call computes required storage size
  if (d_temp_storage == nullptr) {
    cub::DeviceReduce::Reduce(nullptr, temp_storage_bytes,
                              input, output, n,
                              SumInt32(), 0);
    cudaMalloc(&d_temp_storage, temp_storage_bytes);
  }

  for (int iter = 0; iter < iters; ++iter) {
    cub::DeviceReduce::Reduce(d_temp_storage, temp_storage_bytes,
                              input, output, n,
                              SumInt32(), 0);
  }
}