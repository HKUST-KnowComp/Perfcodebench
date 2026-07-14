#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
  __shared__ unsigned int shared_hist[256 * 8];
  
  const int tid = threadIdx.x;
  const int warp_id = tid / 32;
  const int lane_id = tid % 32;
  
  // Initialize shared memory - each thread initializes multiple bins
  for (int i = tid; i < 256 * 8; i += blockDim.x) {
    shared_hist[i] = 0;
  }
  __syncthreads();
  
  // Each warp gets its own private histogram section
  unsigned int* my_hist = shared_hist + warp_id * 256;
  
  // Process input with vectorized loads when possible
  int idx = blockIdx.x * blockDim.x + tid;
  const int stride = blockDim.x * gridDim.x;
  
  // Vectorized processing for aligned portions
  if (((size_t)input & 3) == 0 && idx * 4 < n) {
    const unsigned int* input_vec = reinterpret_cast<const unsigned int*>(input);
    int vec_idx = idx;
    int vec_n = n / 4;
    
    for (; vec_idx < vec_n; vec_idx += stride) {
      unsigned int val = input_vec[vec_idx];
      unsigned char b0 = val & 0xFF;
      unsigned char b1 = (val >> 8) & 0xFF;
      unsigned char b2 = (val >> 16) & 0xFF;
      unsigned char b3 = (val >> 24) & 0xFF;
      
      my_hist[b0]++;
      my_hist[b1]++;
      my_hist[b2]++;
      my_hist[b3]++;
    }
    
    // Handle remaining bytes
    idx = vec_n * 4 + tid;
  }
  
  // Process remaining elements
  for (; idx < n; idx += stride) {
    my_hist[input[idx]]++;
  }
  
  __syncthreads();
  
  // Reduce across warps in shared memory
  if (warp_id == 0) {
    for (int bin = lane_id; bin < 256; bin += 32) {
      unsigned int sum = 0;
      for (int w = 0; w < 8; w++) {
        sum += shared_hist[w * 256 + bin];
      }
      if (sum > 0) {
        atomicAdd(output + bin, sum);
      }
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
