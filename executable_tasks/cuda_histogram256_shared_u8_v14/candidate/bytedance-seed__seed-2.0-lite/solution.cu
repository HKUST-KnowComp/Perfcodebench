#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
  __shared__ unsigned int s_hist[256];
  // Initialize block-local shared histogram to zero
  for (int i = threadIdx.x; i < 256; i += blockDim.x) {
    s_hist[i] = 0;
  }
  __syncthreads();

  // Grid-stride loop to process all input elements
  for (int idx = blockIdx.x * blockDim.x + threadIdx.x; idx < n; idx += blockDim.x * gridDim.x) {
    const unsigned char val = input[idx];
    atomicAdd(&s_hist[val], 1U);
  }
  __syncthreads();

  // Accumulate block-local counts to global output histogram
  for (int i = threadIdx.x; i < 256; i += blockDim.x) {
    atomicAdd(output + i, s_hist[i]);
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