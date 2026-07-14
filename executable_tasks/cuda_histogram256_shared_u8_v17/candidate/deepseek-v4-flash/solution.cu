#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
  __shared__ unsigned int s_hist[256];

  // Initialize shared histogram to zero (each thread handles one bin)
  s_hist[threadIdx.x] = 0;
  __syncthreads();

  // Grid-stride loop: each thread processes multiple elements
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  int stride = blockDim.x * gridDim.x;
  for (; idx < n; idx += stride) {
    atomicAdd(&s_hist[input[idx]], 1U);
  }
  __syncthreads();

  // Write local histogram to global output (one thread per bin)
  if (threadIdx.x < 256) {
    atomicAdd(&output[threadIdx.x], s_hist[threadIdx.x]);
  }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
  constexpr int kBlock = 256;
  // Use up to 1024 blocks, but no more than needed to cover the input
  int grid = (n + kBlock - 1) / kBlock;
  if (grid > 1024) grid = 1024;
  if (grid < 1) grid = 1;

  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, 256 * sizeof(unsigned int));
    histogram_shared_kernel<<<grid, kBlock>>>(input, output, n);
  }
}