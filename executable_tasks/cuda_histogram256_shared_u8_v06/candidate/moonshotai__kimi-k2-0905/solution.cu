#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* __restrict__ input,
                                          unsigned int* __restrict__ output,
                                          int n) {
  __shared__ unsigned int warp_hist[256 * 8];   // 8 warps * 256 bins
  const int lane_id   = threadIdx.x & 31;
  const int warp_id   = threadIdx.x >> 5;
  const int warps_per_block = 8;

  // zero the shared buffer
  for (int i = threadIdx.x; i < 256 * warps_per_block; i += blockDim.x)
    warp_hist[i] = 0;
  __syncthreads();

  // Stage 1: per-warp histogram in shared memory
  const int tid = blockIdx.x * blockDim.x + threadIdx.x;
  for (int idx = tid; idx < n; idx += gridDim.x * blockDim.x) {
    unsigned char bin = input[idx];
    atomicAdd(&warp_hist[bin * warps_per_block + warp_id], 1u);
  }
  __syncthreads();

  // Stage 2: reduce 8 partial histograms to one per CTA
  for (int bin = threadIdx.x; bin < 256; bin += blockDim.x) {
    unsigned int sum = 0;
    #pragma unroll
    for (int w = 0; w < warps_per_block; ++w)
      sum += warp_hist[bin * warps_per_block + w];
    atomicAdd(output + bin, sum);
  }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
  constexpr int kBlock = 256;
  constexpr int kGrid  = 1024;          // 1024 CTAs × 256 threads = 262k threads
  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, 256 * sizeof(unsigned int));
    histogram_shared_kernel<<<kGrid, kBlock>>>(input, output, n);
  }
}