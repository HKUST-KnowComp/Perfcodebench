#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kBins = 256;
constexpr int kBlock = 256;

__global__ void histogram256_shared_kernel(const unsigned char* __restrict__ input,
                                           unsigned int* __restrict__ output,
                                           int n) {
  __shared__ unsigned int smem[kBins];

  // Zero shared histogram.
  for (int i = threadIdx.x; i < kBins; i += blockDim.x) {
    smem[i] = 0;
  }
  __syncthreads();

  const int tid = blockIdx.x * blockDim.x + threadIdx.x;
  const int stride = blockDim.x * gridDim.x;

  // Grid-stride loop with 4-byte unrolling to reduce loop/control overhead.
  int idx = tid;
  const int n4 = n & ~3;
  for (; idx < n4; idx += stride) {
    unsigned char v0 = input[idx];
    atomicAdd(&smem[v0], 1U);

    int idx1 = idx + stride;
    if (idx1 < n4) {
      unsigned char v1 = input[idx1];
      atomicAdd(&smem[v1], 1U);
    }

    int idx2 = idx1 + stride;
    if (idx2 < n4) {
      unsigned char v2 = input[idx2];
      atomicAdd(&smem[v2], 1U);
    }

    int idx3 = idx2 + stride;
    if (idx3 < n4) {
      unsigned char v3 = input[idx3];
      atomicAdd(&smem[v3], 1U);
    }
  }

  // Tail handling for any remaining elements not covered above.
  for (int i = idx; i < n; i += stride) {
    atomicAdd(&smem[input[i]], 1U);
  }

  __syncthreads();

  // Merge per-block shared histogram into global histogram.
  for (int bin = threadIdx.x; bin < kBins; bin += blockDim.x) {
    unsigned int count = smem[bin];
    if (count) atomicAdd(output + bin, count);
  }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
  if (iters <= 0) return;

  int device = 0;
  cudaGetDevice(&device);
  cudaDeviceProp prop;
  cudaGetDeviceProperties(&prop, device);

  int sm_count = prop.multiProcessorCount > 0 ? prop.multiProcessorCount : 1;
  int grid = sm_count * 8;
  if (grid < 1) grid = 1;
  if (grid > 4096) grid = 4096;

  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, kBins * sizeof(unsigned int));
    histogram256_shared_kernel<<<grid, kBlock>>>(input, output, n);
  }
}
