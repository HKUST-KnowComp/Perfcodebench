#include "interface.h"

#include <cub/cub.cuh>
#include <cuda_runtime.h>

namespace {

__global__ void atomic_store(int* ptr, int value) {
  atomicExch(ptr, value);
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  constexpr std::size_t temp_bytes = 1 << 20;  // 1 MiB
  static void* d_temp = [] {
    void* p = nullptr;
    cudaMalloc(&p, temp_bytes);
    return p;
  }();

  for (int iter = 0; iter < iters; ++iter) {
    std::size_t actual_bytes = temp_bytes;
    cub::DeviceReduce::Sum(d_temp, actual_bytes,
                           input, output, n);
    // cub::DeviceReduce::Sum leaves the result in *output, no extra step needed.
  }
}