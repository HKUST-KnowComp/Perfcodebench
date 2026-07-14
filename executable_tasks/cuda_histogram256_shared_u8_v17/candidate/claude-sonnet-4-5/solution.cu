#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
  __shared__ unsigned int shared_hist[256];
  
  // Initialize shared memory
  for (int i = threadIdx.x; i < 256; i += blockDim.x) {
    shared_hist[i] = 0;
  }
  __syncthreads();
  
  // Process input with vectorized loads (4 bytes at a time)
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  int stride = blockDim.x * gridDim.x;
  
  // Vectorized processing for aligned portion
  int n_vec = n / 4;
  const uint32_t* input_vec = reinterpret_cast<const uint32_t*>(input);
  
  for (int i = idx; i < n_vec; i += stride) {
    uint32_t val = input_vec[i];
    unsigned char b0 = val & 0xFF;
    unsigned char b1 = (val >> 8) & 0xFF;
    unsigned char b2 = (val >> 16) & 0xFF;
    unsigned char b3 = (val >> 24) & 0xFF;
    
    atomicAdd(&shared_hist[b0], 1U);
    atomicAdd(&shared_hist[b1], 1U);
    atomicAdd(&shared_hist[b2], 1U);
    atomicAdd(&shared_hist[b3], 1U);
  }
  
  // Handle remaining bytes
  for (int i = n_vec * 4 + idx; i < n; i += stride) {
    atomicAdd(&shared_hist[input[i]], 1U);
  }
  
  __syncthreads();
  
  // Commit shared histogram to global memory
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
  const int grid = 256;
  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, 256 * sizeof(unsigned int));
    histogram_shared_kernel<<<grid, kBlock>>>(input, output, n);
  }
}
