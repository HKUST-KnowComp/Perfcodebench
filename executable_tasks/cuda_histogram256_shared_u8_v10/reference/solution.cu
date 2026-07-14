#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
  __shared__ unsigned int local[256];
  for (int bin = threadIdx.x; bin < 256; bin += blockDim.x) {
    local[bin] = 0;
  }
  __syncthreads();

  for (int idx = blockIdx.x * blockDim.x + threadIdx.x; idx < n; idx += blockDim.x * gridDim.x) {
    atomicAdd(local + input[idx], 1U);
  }
  __syncthreads();

  for (int bin = threadIdx.x; bin < 256; bin += blockDim.x) {
    atomicAdd(output + bin, local[bin]);
  }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
  constexpr int kBlock = 256;
  const int grid = 512;
  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, 256 * sizeof(unsigned int));
    histogram_shared_kernel<<<grid, kBlock>>>(input, output, n);
  }
}
