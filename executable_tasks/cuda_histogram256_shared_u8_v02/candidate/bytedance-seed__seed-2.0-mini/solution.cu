#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
  extern __shared__ unsigned int s_hist[];
  
  const int tid = threadIdx.x;
  s_hist[tid] = 0;
  __syncthreads();

  for (int idx = blockIdx.x * blockDim.x + tid; idx < n; idx += blockDim.x * gridDim.x) {
    const unsigned char val = input[idx];
    atomicAdd(&s_hist[val], 1U);
  }
  __syncthreads();

  atomicAdd(&output[tid], s_hist[tid]);
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
  constexpr int kBlock = 256;
  const int grid = 1024;
  const size_t shared_mem = 256 * sizeof(unsigned int);

  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, 256 * sizeof(unsigned int));
    histogram_shared_kernel<<<grid, kBlock, shared_mem>>>(input, output, n);
  }
}