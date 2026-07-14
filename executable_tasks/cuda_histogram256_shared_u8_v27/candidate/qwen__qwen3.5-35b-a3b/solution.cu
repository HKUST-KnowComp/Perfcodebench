#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
    __shared__ unsigned int shared_counts[256];
    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    // Initialize shared memory
    if (tid == 0) {
        for (int i = 0; i < 256; ++i) {
            shared_counts[i] = 0;
        }
    }
    __syncthreads();

    // Process elements
    for (int i = idx; i < n; i += stride) {
        unsigned char val = input[i];
        atomicAdd(&shared_counts[val], 1);
    }
    __syncthreads();

    // Add shared counts to global output
    atomicAdd(&output[tid], shared_counts[tid]);
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