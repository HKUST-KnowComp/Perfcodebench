#include "interface.h"
#include <cub/cub.cuh>
#include <cuda_runtime.h>
#include <cstdint>

namespace {

struct SumInt32 {
  __device__ __forceinline__ int operator()(const int &a, const int &b) const {
    return a + b;
  }
};

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  // Temporary storage is reused across iterations to avoid re-allocation overhead.
  void* d_temp_storage = nullptr;
  size_t temp_storage_bytes = 0;

  // Query required temporary storage size once.
  cub::DeviceReduce::Reduce(d_temp_storage, temp_storage_bytes,
                            input, output, n, SumInt32(), int{0});
  cudaMalloc(&d_temp_storage, temp_storage_bytes);

  for (int iter = 0; iter < iters; ++iter) {
    cub::DeviceReduce::Reduce(d_temp_storage, temp_storage_bytes,
                              input, output, n, SumInt32(), int{0});
  }

  cudaFree(d_temp_storage);
}