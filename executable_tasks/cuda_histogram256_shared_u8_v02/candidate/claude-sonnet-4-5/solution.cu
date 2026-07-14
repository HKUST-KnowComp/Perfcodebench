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
  
  // Accumulate into shared memory with grid-stride loop
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  int stride = blockDim.x * gridDim.x;
  
  for (int i = idx; i < n; i += stride) {
    unsigned char val = input[i];
    atomicAdd(&shared_hist[val], 1U);
  }
  __syncthreads();
  
  // Merge shared histogram to global memory
  for (int i = threadIdx.x; i < 256; i += blockDim.x) {
    unsigned int count = shared_hist[i];
    if (count > 0) {
      atomicAdd(&output[i], count);
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
