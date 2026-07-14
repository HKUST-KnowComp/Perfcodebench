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
  
  // Compute per-thread histogram offset (interleave by warp)
  const int warp_id = threadIdx.x / 32;
  const int offset = (warp_id % 4) * 256;
  
  // Process input with coalesced reads
  for (int idx = blockIdx.x * blockDim.x + threadIdx.x; idx < n; idx += blockDim.x * gridDim.x) {
    unsigned char val = input[idx];
    atomicAdd(&shared_hist[offset + val], 1U);
  }
  __syncthreads();
  
  // Reduce shared memory to global (each thread handles multiple bins)
  for (int bin = threadIdx.x; bin < 256; bin += blockDim.x) {
    unsigned int sum = 0;
    #pragma unroll
    for (int i = 0; i < 4; ++i) {
      sum += shared_hist[i * 256 + bin];
    }
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
