#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_u8(const unsigned char* input, unsigned int* output, int n) {
  __shared__ unsigned int sh_hist[256];
  const int bin = threadIdx.x;

  // Initialize shared histogram to zero
  sh_hist[bin] = 0;
  __syncthreads();

  // Grid-stride loop to process all input elements
  for (int idx = blockIdx.x * blockDim.x + threadIdx.x; idx < n; idx += blockDim.x * gridDim.x) {
    const unsigned char c = input[idx];
    atomicAdd(&sh_hist[c], 1U);
  }
  __syncthreads();

  // Aggregate local block histogram to global output
  atomicAdd(&output[bin], sh_hist[bin]);
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
  constexpr int kBlockSize = 256;
  const int grid_size = (n <= 0) ? 0 : (n + kBlockSize - 1) / kBlockSize;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset output histogram to zero for this iteration
    cudaMemset(output, 0, 256 * sizeof(unsigned int));
    // Launch kernel only if there are valid elements to process
    if (grid_size > 0) {
      histogram_shared_u8<<<grid_size, kBlockSize>>>(input, output, n);
    }
  }
}