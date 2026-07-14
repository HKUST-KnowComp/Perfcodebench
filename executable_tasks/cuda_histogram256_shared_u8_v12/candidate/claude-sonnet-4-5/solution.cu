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
  
  // Process input with vectorized loads and unrolling
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  int stride = blockDim.x * gridDim.x;
  
  // Vectorized processing for aligned portions
  if (n >= 16 && ((unsigned long long)input % 16 == 0)) {
    int vec_n = n / 16;
    const uint4* input_vec = reinterpret_cast<const uint4*>(input);
    
    for (int i = idx; i < vec_n; i += stride) {
      uint4 data = input_vec[i];
      unsigned char* bytes = reinterpret_cast<unsigned char*>(&data);
      
      #pragma unroll
      for (int j = 0; j < 16; j++) {
        atomicAdd(&shared_hist[bytes[j]], 1U);
      }
    }
    
    // Handle remaining bytes
    for (int i = vec_n * 16 + idx; i < n; i += stride) {
      atomicAdd(&shared_hist[input[i]], 1U);
    }
  } else {
    // Fallback for unaligned or small inputs with 4-way unrolling
    int i = idx;
    for (; i + 3 * stride < n; i += 4 * stride) {
      atomicAdd(&shared_hist[input[i]], 1U);
      atomicAdd(&shared_hist[input[i + stride]], 1U);
      atomicAdd(&shared_hist[input[i + 2 * stride]], 1U);
      atomicAdd(&shared_hist[input[i + 3 * stride]], 1U);
    }
    for (; i < n; i += stride) {
      atomicAdd(&shared_hist[input[i]], 1U);
    }
  }
  
  __syncthreads();
  
  // Reduce to global memory
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
  const int grid = min(1024, (n + kBlock - 1) / kBlock);
  
  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, 256 * sizeof(unsigned int));
    histogram_shared_kernel<<<grid, kBlock>>>(input, output, n);
  }
}
