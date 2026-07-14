#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
  // 4-way interleaved shared memory to reduce bank conflicts
  __shared__ unsigned int shared_hist[256 * 4];
  
  // Initialize shared memory
  for (int i = threadIdx.x; i < 256 * 4; i += blockDim.x) {
    shared_hist[i] = 0;
  }
  __syncthreads();
  
  // Process input with vectorized loads when possible
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  int stride = blockDim.x * gridDim.x;
  
  // Determine which interleaved section this thread uses
  int lane = threadIdx.x & 3;
  int offset = lane * 256;
  
  // Vectorized load path for aligned data
  if (n >= 4 && ((size_t)input & 3) == 0) {
    int vec_n = n / 4;
    const uint4* input_vec = reinterpret_cast<const uint4*>(input);
    
    for (int i = idx; i < vec_n; i += stride) {
      uint4 data = input_vec[i];
      unsigned char bytes[4];
      bytes[0] = data.x & 0xFF;
      bytes[1] = (data.x >> 8) & 0xFF;
      bytes[2] = (data.x >> 16) & 0xFF;
      bytes[3] = (data.x >> 24) & 0xFF;
      
      atomicAdd(&shared_hist[offset + bytes[0]], 1U);
      atomicAdd(&shared_hist[offset + bytes[1]], 1U);
      atomicAdd(&shared_hist[offset + bytes[2]], 1U);
      atomicAdd(&shared_hist[offset + bytes[3]], 1U);
    }
    
    // Handle remainder
    for (int i = vec_n * 4 + idx; i < n; i += stride) {
      atomicAdd(&shared_hist[offset + input[i]], 1U);
    }
  } else {
    // Scalar path
    for (int i = idx; i < n; i += stride) {
      atomicAdd(&shared_hist[offset + input[i]], 1U);
    }
  }
  
  __syncthreads();
  
  // Reduce interleaved sections and write to global memory
  for (int bin = threadIdx.x; bin < 256; bin += blockDim.x) {
    unsigned int sum = shared_hist[bin] + shared_hist[256 + bin] + 
                       shared_hist[512 + bin] + shared_hist[768 + bin];
    if (sum > 0) {
      atomicAdd(&output[bin], sum);
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
