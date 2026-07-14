#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
  __shared__ unsigned int hist_s[256];
  
  // Initialize shared histogram to zero
  if (threadIdx.x < 256) {
    hist_s[threadIdx.x] = 0U;
  }
  __syncthreads();

  // Grid-stride loop to process all input elements
  const int block_stride = blockDim.x * gridDim.x;
  for (int idx = blockIdx.x * blockDim.x + threadIdx.x; idx < n; idx += block_stride) {
    const unsigned char val = input[idx];
    atomicAdd(&hist_s[val], 1U);
  }

  // Wait for all block threads to finish updating shared histogram
  __syncthreads();

  // Merge block's shared histogram into global output
  if (threadIdx.x < 256) {
    atomicAdd(&output[threadIdx.x], hist_s[threadIdx.x]);
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