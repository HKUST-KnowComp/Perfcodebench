#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kBins = 256;
constexpr int kBlock = 256;

__global__ void histogram256_shared_kernel(const unsigned char* __restrict__ input,
                                           unsigned int* __restrict__ output,
                                           int n) {
  __shared__ unsigned int shist[kBins];

  // Zero shared histogram.
  for (int i = threadIdx.x; i < kBins; i += blockDim.x) {
    shist[i] = 0;
  }
  __syncthreads();

  const int global_tid = blockIdx.x * blockDim.x + threadIdx.x;
  const int stride = blockDim.x * gridDim.x;

  // Grid-stride loop with light unrolling to improve memory throughput.
  int idx = global_tid;
  const int unroll = 4;
  const int step = stride * unroll;

  for (; idx + step - stride < n; idx += step) {
    unsigned char v0 = input[idx];
    unsigned char v1 = input[idx + stride];
    unsigned char v2 = input[idx + 2 * stride];
    unsigned char v3 = input[idx + 3 * stride];
    atomicAdd(&shist[v0], 1U);
    atomicAdd(&shist[v1], 1U);
    atomicAdd(&shist[v2], 1U);
    atomicAdd(&shist[v3], 1U);
  }

  for (; idx < n; idx += stride) {
    atomicAdd(&shist[input[idx]], 1U);
  }
  __syncthreads();

  // Merge per-block shared histogram into global histogram.
  for (int i = threadIdx.x; i < kBins; i += blockDim.x) {
    unsigned int v = shist[i];
    if (v) atomicAdd(output + i, v);
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
      histogram256_shared_kernel<<<grid, kBlock>>>(input, output, n);
    }
  }
}
