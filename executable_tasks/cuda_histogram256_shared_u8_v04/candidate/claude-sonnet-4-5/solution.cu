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
  
  // Process input with vectorized loads
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  int stride = blockDim.x * gridDim.x;
  
  // Vectorized processing for aligned portions
  if (n >= 4 && idx * 4 < n) {
    int vec_n = n / 4;
    const uint32_t* input_vec = reinterpret_cast<const uint32_t*>(input);
    
    for (int i = idx; i < vec_n; i += stride) {
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
    
    // Handle remainder
    int remainder_start = vec_n * 4;
    for (int i = remainder_start + threadIdx.x + blockIdx.x * blockDim.x; i < n; i += stride) {
      atomicAdd(&shared_hist[input[i]], 1U);
    }
  } else {
    // Fallback for small n
    for (int i = idx; i < n; i += stride) {
      atomicAdd(&shared_hist[input[i]], 1U);
    }
  }
  
  __syncthreads();
  
  // Commit to global memory
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
  const int grid = min(1024, (n + kBlock * 4 - 1) / (kBlock * 4));
  
  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, 256 * sizeof(unsigned int));
    histogram_shared_kernel<<<grid, kBlock>>>(input, output, n);
  }
}
