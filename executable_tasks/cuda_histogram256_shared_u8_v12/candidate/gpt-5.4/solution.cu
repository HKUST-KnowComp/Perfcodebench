#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kBins = 256;
constexpr int kBlock = 256;

__global__ void histogram256_shared_kernel(const unsigned char* __restrict__ input,
                                           unsigned int* __restrict__ output,
                                           int n) {
  __shared__ unsigned int smem[kBins];

  const int tid = threadIdx.x;

  smem[tid] = 0;
  __syncthreads();

  const int global_tid = blockIdx.x * blockDim.x + tid;
  const int stride = blockDim.x * gridDim.x;

  int idx = global_tid;

  // Process 4 bytes per loop iteration when possible to reduce loop/control overhead.
  for (; idx <= n - 4 * stride; idx += 4 * stride) {
    unsigned char v0 = input[idx];
    unsigned char v1 = input[idx + stride];
    unsigned char v2 = input[idx + 2 * stride];
    unsigned char v3 = input[idx + 3 * stride];

    atomicAdd(&smem[v0], 1U);
    atomicAdd(&smem[v1], 1U);
    atomicAdd(&smem[v2], 1U);
    atomicAdd(&smem[v3], 1U);
  }

  for (; idx < n; idx += stride) {
    atomicAdd(&smem[input[idx]], 1U);
  }

  __syncthreads();

  atomicAdd(output + tid, smem[tid]);
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
  if (iters <= 0) return;

  int device = 0;
  cudaGetDevice(&device);

  cudaDeviceProp prop;
  cudaGetDeviceProperties(&prop, device);

  int grid = prop.multiProcessorCount * 8;
  if (grid < 1) grid = 1;

  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, kBins * sizeof(unsigned int));
    histogram256_shared_kernel<<<grid, kBlock>>>(input, output, n);
  }
}
