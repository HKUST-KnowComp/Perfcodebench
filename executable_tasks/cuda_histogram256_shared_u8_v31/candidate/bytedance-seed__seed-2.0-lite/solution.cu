#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
  __shared__ unsigned int hist[256];
  // Initialize per-block shared histogram to zero
  if (threadIdx.x < 256) {
    hist[threadIdx.x] = 0U;
  }
  __syncthreads();

  // Process main input with vectorized 4-byte loads to maximize bandwidth utilization
  const uint4* input_4 = reinterpret_cast<const uint4*>(input);
  const int total_vec4 = n / 4;
  for (int idx = blockIdx.x * blockDim.x + threadIdx.x; idx < total_vec4; idx += blockDim.x * gridDim.x) {
    uint4 bytes = input_4[idx];
    atomicAdd(&hist[bytes.x], 1U);
    atomicAdd(&hist[bytes.y], 1U);
    atomicAdd(&hist[bytes.z], 1U);
    atomicAdd(&hist[bytes.w], 1U);
  }

  // Handle any remaining trailing bytes not aligned to 4-byte boundaries
  const int rem_start = total_vec4 * 4;
  for (int idx = rem_start + threadIdx.x; idx < n; idx += blockDim.x * gridDim.x) {
    unsigned char b = input[idx];
    atomicAdd(&hist[b], 1U);
  }

  __syncthreads();
  // Merge block's local histogram into global output, minimal global atomics
  if (threadIdx.x < 256) {
    atomicAdd(&output[threadIdx.x], hist[threadIdx.x]);
  }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
  constexpr int kBlock = 256;
  const int grid = 8192; // Sufficient grid size to saturate modern GPU multiprocessors
  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, 256 * sizeof(unsigned int));
    histogram_shared_kernel<<<grid, kBlock>>>(input, output, n);
  }
}