#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram256_warp_priv_kernel(const unsigned char* __restrict__ input,
                                              unsigned int* __restrict__ output,
                                              int n) {
  extern __shared__ unsigned int s_hist[];  // warps_per_block * 256 bins

  const int tid   = threadIdx.x;
  const int lane  = tid & 31;        // thread index within warp
  const int warp  = tid >> 5;        // warp index within block
  const int warps = blockDim.x >> 5; // warps per block

  unsigned int* my_hist = s_hist + warp * 256;

  // Initialize this warp's histogram
  for (int bin = lane; bin < 256; bin += 32) {
    my_hist[bin] = 0u;
  }
  __syncthreads();

  // Grid-stride loop over input data
  int idx = blockIdx.x * blockDim.x + tid;
  const int stride = blockDim.x * gridDim.x;

  for (; idx < n; idx += stride) {
    unsigned int b = static_cast<unsigned int>(input[idx]);
    atomicAdd(&my_hist[b], 1u);
  }
  __syncthreads();

  // Reduce per-warp histograms to global output
  if (tid < 256) {
    unsigned int sum = 0u;
    for (int w = 0; w < warps; ++w) {
      sum += s_hist[w * 256 + tid];
    }
    if (sum) {
      atomicAdd(output + tid, sum);
    }
  }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
  const int block = 256;
  const int grid  = 1024;
  const size_t shmem = static_cast<size_t>(block / 32) * 256u * sizeof(unsigned int);

  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, 256 * sizeof(unsigned int));
    histogram256_warp_priv_kernel<<<grid, block, shmem>>>(input, output, n);
  }
}
