#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void __launch_bounds__(256) histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
  __shared__ unsigned int s_hist[256];

  // Each thread initializes exactly one bin
  s_hist[threadIdx.x] = 0;
  __syncthreads();

  // Grid-stride loop for coalesced global reads
  int stride = blockDim.x * gridDim.x;
  for (int idx = blockIdx.x * blockDim.x + threadIdx.x; idx < n; idx += stride) {
    atomicAdd(&s_hist[input[idx]], 1U);
  }
  __syncthreads();

  // Merge local block histogram into global output
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
