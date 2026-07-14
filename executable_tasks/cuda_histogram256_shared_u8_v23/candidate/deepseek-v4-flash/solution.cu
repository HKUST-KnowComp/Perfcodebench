#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
  __shared__ unsigned int shist[256];

  // Initialize shared memory histogram to zero
  if (threadIdx.x < 256) {
    shist[threadIdx.x] = 0;
  }
  __syncthreads();

  // Strided loop over input elements
  int tid = blockIdx.x * blockDim.x + threadIdx.x;
  int stride = blockDim.x * gridDim.x;
  for (int i = tid; i < n; i += stride) {
    unsigned char val = __ldg(&input[i]);
    atomicAdd(&shist[val], 1U);
  }
  __syncthreads();

  // Combine per-block histogram into global output
  if (threadIdx.x < 256) {
    atomicAdd(&output[threadIdx.x], shist[threadIdx.x]);
  }
}

}  // anonymous namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
  constexpr int kBlock = 256;
  const int grid = min(1024, (n + kBlock - 1) / kBlock);

  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, 256 * sizeof(unsigned int));
    histogram_shared_kernel<<<grid, kBlock>>>(input, output, n);
  }
}
