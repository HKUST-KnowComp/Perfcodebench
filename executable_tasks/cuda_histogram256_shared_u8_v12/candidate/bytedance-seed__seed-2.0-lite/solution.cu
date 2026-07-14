#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
  // Per-block private histogram in low-latency shared memory
  __shared__ unsigned int s_hist[256];

  // Initialize shared histogram to zero (block size 256, each thread zeros 1 bin)
  if (threadIdx.x < 256) {
    s_hist[threadIdx.x] = 0U;
  }
  __syncthreads();

  // Grid-stride loop to process all input with coalesced memory access
  const int block_stride = blockDim.x * gridDim.x;
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  while (idx < n) {
    const unsigned char val = input[idx];
    atomicAdd(&s_hist[val], 1U);
    idx += block_stride;
  }
  __syncthreads();

  // Merge block's shared histogram into global output (only 256 global atomics per block)
  if (threadIdx.x < 256) {
    atomicAdd(&output[threadIdx.x], s_hist[threadIdx.x]);
  }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
  constexpr int kBlock = 256;
  const int grid = 1024;
  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, 256 * sizeof(unsigned int));
    histogram_shared_kernel<<<grid, kBlock>>>(input, output, n);
  }
}