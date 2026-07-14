#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kBins = 256;
constexpr int kBlock = 256;

__global__ void histogram256_shared_kernel(const unsigned char* __restrict__ input,
                                           unsigned int* __restrict__ output,
                                           int n) {
  __shared__ unsigned int shist[kBins];

  const int tid = threadIdx.x;
  const int stride = blockDim.x * gridDim.x;
  int idx = blockIdx.x * blockDim.x + tid;

  shist[tid] = 0;
  __syncthreads();

  const int limit4 = n & ~3;
  for (; idx < limit4; idx += stride) {
    uchar4 v = *reinterpret_cast<const uchar4*>(input + idx);
    atomicAdd(&shist[v.x], 1U);
    atomicAdd(&shist[v.y], 1U);
    atomicAdd(&shist[v.z], 1U);
    atomicAdd(&shist[v.w], 1U);
  }

  for (; idx < n; idx += stride) {
    atomicAdd(&shist[input[idx]], 1U);
  }

  __syncthreads();
  atomicAdd(output + tid, shist[tid]);
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
  if (grid > 65535) grid = 65535;

  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, kBins * sizeof(unsigned int));
    histogram256_shared_kernel<<<grid, kBlock>>>(input, output, n);
  }
}
