#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_atomic_kernel(const unsigned char* input, unsigned int* output, int n) {
  __shared__ unsigned int block_hist[256];

  // Parallelize shared memory initialization to zero across block threads
  if (threadIdx.x < 256) {
    block_hist[threadIdx.x] = 0U;
  }
  __syncthreads();

  // Process all input elements assigned to this thread
  for (int idx = blockIdx.x * blockDim.x + threadIdx.x; idx < n; idx += blockDim.x * gridDim.x) {
    const unsigned char bin = input[idx];
    atomicAdd(&block_hist[bin], 1U);
  }
  __syncthreads();

  // Merge block's local histogram into global output with minimal global atomics
  if (threadIdx.x < 256) {
    atomicAdd(&output[threadIdx.x], block_hist[threadIdx.x]);
  }
}

} // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
  constexpr int kBlock = 256;
  const int grid = 1024;
  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, 256 * sizeof(unsigned int));
    histogram_shared_atomic_kernel<<<grid, kBlock>>>(input, output, n);
  }
}