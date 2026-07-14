#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
  __shared__ unsigned int s_hist[256][4];
  int sub = threadIdx.x & 3;

  // Initialize shared sub-histograms
  for (int i = threadIdx.x; i < 256; i += blockDim.x) {
    s_hist[i][0] = 0;
    s_hist[i][1] = 0;
    s_hist[i][2] = 0;
    s_hist[i][3] = 0;
  }
  __syncthreads();

  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  int stride = blockDim.x * gridDim.x;

  // Grid-stride loop: accumulate into shared memory
  for (int i = idx; i < n; i += stride) {
    atomicAdd(&s_hist[input[i]][sub], 1);
  }
  __syncthreads();

  // Reduce sub-histograms and flush to global output
  for (int i = threadIdx.x; i < 256; i += blockDim.x) {
    unsigned int sum = s_hist[i][0] + s_hist[i][1] + s_hist[i][2] + s_hist[i][3];
    if (sum > 0) {
      atomicAdd(&output[i], sum);
    }
  }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
  constexpr int kBlock = 256;
  const int grid = 1024;
  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, 256 * sizeof(unsigned int));
    histogram_shared_kernel<<<grid, kBlock>>>(input, output, n);
  }
}