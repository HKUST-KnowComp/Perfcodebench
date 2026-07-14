#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_atomic_kernel(const unsigned char* input, unsigned int* output, int n) {
  __shared__ unsigned int s_hist[256];

  // Initialize per-block shared histogram to zero
  for (int i = threadIdx.x; i < 256; i += blockDim.x) {
    s_hist[i] = 0;
  }
  __syncthreads();

  // Accumulate counts to block-local shared histogram
  for (int idx = blockIdx.x * blockDim.x + threadIdx.x; idx < n; idx += blockDim.x * gridDim.x) {
    atomicAdd(&s_hist[input[idx]], 1U);
  }
  __syncthreads();

  // Merge block's final histogram into global output
  for (int i = threadIdx.x; i < 256; i += blockDim.x) {
    atomicAdd(&output[i], s_hist[i]);
  }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
  constexpr int kBlock = 256;
  const int grid = 1024;
  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, 256 * sizeof(unsigned int));
    histogram_shared_atomic_kernel<<<grid, kBlock>>>(input, output, n);
  }
}