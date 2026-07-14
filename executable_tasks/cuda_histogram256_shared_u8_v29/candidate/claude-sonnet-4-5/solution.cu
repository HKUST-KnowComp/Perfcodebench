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
  
  // Process input with vectorized loads
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  int stride = blockDim.x * gridDim.x;
  
  // Vectorized processing for aligned portions
  int vec_end = (n / 16) * 16;
  for (int i = idx * 4; i < vec_end; i += stride * 4) {
    if (i + 3 < n) {
      uint4 data = *reinterpret_cast<const uint4*>(input + i);
      unsigned char* bytes = reinterpret_cast<unsigned char*>(&data);
      
      #pragma unroll
      for (int j = 0; j < 16; ++j) {
        unsigned char val = bytes[j];
        atomicAdd(&shared_hist[val * 4 + (threadIdx.x & 3)], 1U);
      }
    }
  }
  
  // Handle remaining elements
  for (int i = vec_end + idx; i < n; i += stride) {
    unsigned char val = input[i];
    atomicAdd(&shared_hist[val * 4 + (threadIdx.x & 3)], 1U);
  }
  
  __syncthreads();
  
  // Reduce shared memory to global memory
  for (int bin = threadIdx.x; bin < 256; bin += blockDim.x) {
    unsigned int sum = shared_hist[bin * 4] + shared_hist[bin * 4 + 1] + 
                       shared_hist[bin * 4 + 2] + shared_hist[bin * 4 + 3];
    if (sum > 0) {
      atomicAdd(&output[bin], sum);
    }
  }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
  constexpr int kBlock = 256;
  const int grid = min(1024, (n + kBlock * 16 - 1) / (kBlock * 16));
  
  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, 256 * sizeof(unsigned int));
    histogram_shared_kernel<<<grid, kBlock>>>(input, output, n);
  }
}
