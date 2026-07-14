#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
  __shared__ unsigned int s_hist[256];

  // Initialize shared memory histogram to zero
  for (int i = threadIdx.x; i < 256; i += blockDim.x) {
    s_hist[i] = 0;
  }
  __syncthreads();

  // Process input elements using grid-stride loop
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  int stride = blockDim.x * gridDim.x;
  while (idx < n) {
    unsigned char val = input[idx];
    atomicAdd(&s_hist[val], 1U);
    idx += stride;
  }
  __syncthreads();

  // Merge block-local histogram into global output
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
