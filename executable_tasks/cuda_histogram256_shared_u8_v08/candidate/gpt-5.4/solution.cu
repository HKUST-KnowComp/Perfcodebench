#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kBins = 256;
constexpr int kBlock = 256;
constexpr int kWarpsPerBlock = kBlock / 32;

__global__ void histogram256_shared_warp_kernel(const unsigned char* __restrict__ input,
                                                unsigned int* __restrict__ output,
                                                int n) {
  __shared__ unsigned int smem[kWarpsPerBlock * kBins];

  const int tid = threadIdx.x;
  const int lane = tid & 31;
  const int warp = tid >> 5;
  const int global_stride = blockDim.x * gridDim.x;
  int idx = blockIdx.x * blockDim.x + tid;

  for (int i = tid; i < kWarpsPerBlock * kBins; i += blockDim.x) {
    smem[i] = 0;
  }
  __syncthreads();

  const unsigned char* ptr = input + idx;
  const int stride4 = global_stride * 4;

  for (; idx + 3 * global_stride < n; idx += stride4, ptr += stride4) {
    unsigned char v0 = ptr[0 * global_stride];
    unsigned char v1 = ptr[1 * global_stride];
    unsigned char v2 = ptr[2 * global_stride];
    unsigned char v3 = ptr[3 * global_stride];

    atomicAdd(&smem[warp * kBins + v0], 1U);
    atomicAdd(&smem[warp * kBins + v1], 1U);
    atomicAdd(&smem[warp * kBins + v2], 1U);
    atomicAdd(&smem[warp * kBins + v3], 1U);
  }

  for (; idx < n; idx += global_stride) {
    atomicAdd(&smem[warp * kBins + input[idx]], 1U);
  }
  __syncthreads();

  for (int bin = tid; bin < kBins; bin += blockDim.x) {
    unsigned int sum = 0;
#pragma unroll
    for (int w = 0; w < kWarpsPerBlock; ++w) {
      sum += smem[w * kBins + bin];
    }
    if (sum) atomicAdd(output + bin, sum);
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
  if (grid > 65535) grid = 65535;

  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, kBins * sizeof(unsigned int));
    if (n > 0) {
      histogram256_shared_warp_kernel<<<grid, kBlock>>>(input, output, n);
    }
  }
}
