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
  
  // Vectorized processing for coalesced access
  int vec_n = n / 4;
  const uint4* input_vec = reinterpret_cast<const uint4*>(input);
  
  for (int i = idx; i < vec_n; i += stride) {
    uint4 data = input_vec[i];
    unsigned char* bytes = reinterpret_cast<unsigned char*>(&data);
    
    // 4-way interleaving: thread T uses bank (T % 4)
    int bank = threadIdx.x & 3;
    atomicAdd(&shared_hist[bytes[0] * 4 + bank], 1U);
    atomicAdd(&shared_hist[bytes[1] * 4 + bank], 1U);
    atomicAdd(&shared_hist[bytes[2] * 4 + bank], 1U);
    atomicAdd(&shared_hist[bytes[3] * 4 + bank], 1U);
  }
  
  // Handle remaining bytes
  for (int i = vec_n * 4 + idx; i < n; i += stride) {
    int bank = threadIdx.x & 3;
    atomicAdd(&shared_hist[input[i] * 4 + bank], 1U);
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
  const int grid = min(1024, (n + kBlock * 4 - 1) / (kBlock * 4));
  
  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, 256 * sizeof(unsigned int));
    histogram_shared_kernel<<<grid, kBlock>>>(input, output, n);
  }
}
