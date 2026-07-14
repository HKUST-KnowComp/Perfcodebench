#include "interface.h"

#include <cub/cub.cuh>
#include <cuda_runtime.h>

#include <cstdint>
#include <cstdio>

namespace {

struct FastDiv {
  unsigned m, s;
  __host__ __device__ FastDiv(unsigned d) {
    for (s = 0; (1U << s) < d; ++s) {}
    m = (1ULL << 32) / d + 1;
  }
  __device__ unsigned operator()(unsigned x) const {
    return (static_cast<uint64_t>(x) * m) >> 32 >> s;
  }
};

__global__ void tail_reduce(const int* partial, int* out, int count) {
  int sum = 0;
  for (int i = threadIdx.x; i < count; i += blockDim.x) {
    sum += partial[i];
  }
  using BlockReduce = cub::BlockReduce<int, 256>;
  __shared__ typename BlockReduce::TempStorage tmp;
  sum = BlockReduce(tmp).Sum(sum);
  if (threadIdx.x == 0) {
    *out = sum;
  }
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  static int* d_out = nullptr;
  static int* d_partials = nullptr;
  static void* d_tmp = nullptr;
  static size_t tmp_bytes = 0;
  static int capacity = 0;

  if (capacity == 0) {
    cudaMalloc(&d_out, sizeof(int));
    capacity = n;
    int blocks = (n + 511) / 512;
    cudaMalloc(&d_partials, blocks * sizeof(int));
    cub::DeviceReduce::Sum(nullptr, tmp_bytes, input, d_partials, n);
    cudaMalloc(&d_tmp, tmp_bytes);
  }

  for (int i = 0; i < iters; ++i) {
    cub::DeviceReduce::Sum(d_tmp, tmp_bytes, input, d_partials, n);
    tail_reduce<<<1, 256>>>(d_partials, d_out, (n + 511) / 512);
  }
  cudaMemcpy(output, d_out, sizeof(int), cudaMemcpyDeviceToHost);
}