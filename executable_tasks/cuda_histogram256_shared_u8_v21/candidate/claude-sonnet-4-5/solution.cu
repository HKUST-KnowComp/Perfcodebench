#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
  __shared__ unsigned int shared_hist[256];
  
  // Initialize shared memory histogram
  for (int i = threadIdx.x; i < 256; i += blockDim.x) {
    shared_hist[i] = 0;
  }
  __syncthreads();
  
  // Compute local histogram with shared memory atomics
  for (int idx = blockIdx.x * blockDim.x + threadIdx.x; idx < n; idx += blockDim.x * gridDim.x) {
    atomicAdd(&shared_hist[input[idx]], 1U);
  }
  __syncthreads();
  
  // Merge shared histogram to global memory
  for (int i = threadIdx.x; i < 256; i += blockDim.x) {
    unsigned int val = shared_hist[i];
    if (val > 0) {
      atomicAdd(&output[i], val);
    }
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
