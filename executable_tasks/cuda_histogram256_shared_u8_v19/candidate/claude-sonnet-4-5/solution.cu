#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
  __shared__ unsigned int shared_hist[256];
  
  // Initialize shared memory histogram
  if (threadIdx.x < 256) {
    shared_hist[threadIdx.x] = 0;
  }
  __syncthreads();
  
  // Compute local histogram with vectorized loads
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  int stride = blockDim.x * gridDim.x;
  
  // Vectorized load path for aligned access
  if (idx * 4 < n) {
    int vec_idx = idx;
    int vec_stride = stride;
    
    for (; vec_idx * 4 + 3 < n; vec_idx += vec_stride) {
      uchar4 data = reinterpret_cast<const uchar4*>(input)[vec_idx];
      atomicAdd(&shared_hist[data.x], 1U);
      atomicAdd(&shared_hist[data.y], 1U);
      atomicAdd(&shared_hist[data.z], 1U);
      atomicAdd(&shared_hist[data.w], 1U);
    }
    
    // Handle remaining elements
    int remaining_idx = vec_idx * 4;
    for (; remaining_idx < n; remaining_idx += stride) {
      atomicAdd(&shared_hist[input[remaining_idx]], 1U);
    }
  }
  
  __syncthreads();
  
  // Merge shared histogram to global memory
  if (threadIdx.x < 256) {
    unsigned int count = shared_hist[threadIdx.x];
    if (count > 0) {
      atomicAdd(&output[threadIdx.x], count);
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
