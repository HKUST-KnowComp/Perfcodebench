#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* __restrict__ input, unsigned int* __restrict__ output, int n) {
  __shared__ unsigned int smem[256];

  // Initialize shared memory to zero (exactly one thread per bin)
  smem[threadIdx.x] = 0;
  __syncthreads();

  // Grid-stride loop to process all input elements efficiently
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  while (idx < n) {
    const unsigned char bin = input[idx];
    atomicAdd(&smem[bin], 1U);
    idx += blockDim.x * gridDim.x;
  }
  __syncthreads();

  // Aggregate local block counts to global histogram
  atomicAdd(&output[threadIdx.x], smem[threadIdx.x]);
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
  constexpr int kBlock = 256;
  constexpr int kGrid = 1024;
  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, 256 * sizeof(unsigned int));
    histogram_shared_kernel<<<kGrid, kBlock>>>(input, output, n);
  }
}