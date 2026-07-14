#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
  __shared__ unsigned int shared_hist[256];
  
  // Initialize shared memory - each thread handles multiple bins
  for (int i = threadIdx.x; i < 256; i += blockDim.x) {
    shared_hist[i] = 0;
  }
  __syncthreads();
  
  // Process input with vectorized loads when possible
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  int stride = blockDim.x * gridDim.x;
  
  // Vectorized processing for aligned portions
  if (n >= 4 && ((unsigned long long)input % 4 == 0)) {
    int vec_end = (n / 4) * 4;
    for (int i = idx * 4; i < vec_end; i += stride * 4) {
      if (i + 3 < vec_end) {
        unsigned int val = *reinterpret_cast<const unsigned int*>(input + i);
        unsigned char b0 = val & 0xFF;
        unsigned char b1 = (val >> 8) & 0xFF;
        unsigned char b2 = (val >> 16) & 0xFF;
        unsigned char b3 = (val >> 24) & 0xFF;
        atomicAdd(&shared_hist[b0], 1U);
        atomicAdd(&shared_hist[b1], 1U);
        atomicAdd(&shared_hist[b2], 1U);
        atomicAdd(&shared_hist[b3], 1U);
      }
    }
    // Handle remainder
    for (int i = vec_end + idx; i < n; i += stride) {
      atomicAdd(&shared_hist[input[i]], 1U);
    }
  } else {
    // Fallback for unaligned or small inputs
    for (int i = idx; i < n; i += stride) {
      atomicAdd(&shared_hist[input[i]], 1U);
    }
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
  const int grid = min(1024, (n + kBlock - 1) / kBlock);
  
  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, 256 * sizeof(unsigned int));
    histogram_shared_kernel<<<grid, kBlock>>>(input, output, n);
  }
}
