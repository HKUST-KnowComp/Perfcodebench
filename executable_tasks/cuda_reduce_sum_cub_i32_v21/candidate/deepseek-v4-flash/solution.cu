#include "interface.h"
#include <cuda_runtime.h>
#include <algorithm>

namespace {

template <int kBlockSize>
__global__ __launch_bounds__(kBlockSize) void reduce_kernel(
    const int* input, int* output, int n) {
  int tid = blockIdx.x * blockDim.x + threadIdx.x;
  int stride = blockDim.x * gridDim.x;
  int sum = 0;
  for (int i = tid; i < n; i += stride) {
    sum += input[i];
  }

  __shared__ int sdata[kBlockSize];
  sdata[threadIdx.x] = sum;
  __syncthreads();

#pragma unroll
  for (int s = kBlockSize / 2; s > 0; s >>= 1) {
    if (threadIdx.x < s) {
      sdata[threadIdx.x] += sdata[threadIdx.x + s];
    }
    __syncthreads();
  }

  if (threadIdx.x == 0) {
    atomicAdd(output, sdata[0]);
  }
}

}  // anonymous namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  constexpr int kBlock = 256;
  int grid = (n + kBlock - 1) / kBlock;
  if (grid > 1024) grid = 1024;

  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, sizeof(int));
    reduce_kernel<kBlock><<<grid, kBlock>>>(input, output, n);
  }
}
