#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* __restrict__ input, unsigned int* __restrict__ output, int n) {
  __shared__ unsigned int s_hist[256];

  s_hist[threadIdx.x] = 0;
  __syncthreads();

  const int stride = blockDim.x * gridDim.x;
  for (int i = blockIdx.x * blockDim.x + threadIdx.x; i < n; i += stride) {
    atomicAdd(&s_hist[input[i]], 1U);
  }
  __syncthreads();

  atomicAdd(&output[threadIdx.x], s_hist[threadIdx.x]);
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
