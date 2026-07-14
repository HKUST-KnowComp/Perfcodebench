#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram_global_atomic_kernel(const unsigned char* input, unsigned int* output, int n) {
  for (int idx = blockIdx.x * blockDim.x + threadIdx.x; idx < n; idx += blockDim.x * gridDim.x) {
    atomicAdd(output + input[idx], 1U);
  }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
  constexpr int kBlock = 256;
  const int grid = 1024;
  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, 256 * sizeof(unsigned int));
    histogram_global_atomic_kernel<<<grid, kBlock>>>(input, output, n);
  }
}
