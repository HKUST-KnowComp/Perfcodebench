#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
  __shared__ unsigned int hist[256];
  
  // Initialize shared memory histogram to zero
  if (threadIdx.x < 256) {
    hist[threadIdx.x] = 0;
  }
  __syncthreads();
  
  // Accumulate input into shared histogram using local atomics
  for (int idx = blockIdx.x * blockDim.x + threadIdx.x; idx < n; idx += blockDim.x * gridDim.x) {
    atomicAdd(&hist[input[idx]], 1U);
  }
  __syncthreads();
  
  // Write shared histogram to global output using atomic adds
  atomicAdd(&output[threadIdx.x], hist[threadIdx.x]);
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
  constexpr int kBlock = 256;
  const int grid = 1024;  // Same as baseline; can be tuned but kept for compatibility
  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, 256 * sizeof(unsigned int));
    histogram_shared_kernel<<<grid, kBlock>>>(input, output, n);
  }
}