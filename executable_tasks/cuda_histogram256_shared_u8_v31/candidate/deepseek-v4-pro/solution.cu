#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
  extern __shared__ unsigned int s_hist[]; // 256 unsigned ints

  int tid = threadIdx.x;
  // Initialize shared histogram cooperatively
  for (int i = tid; i < 256; i += blockDim.x) {
    s_hist[i] = 0;
  }
  __syncthreads();

  int idx = blockIdx.x * blockDim.x + tid;
  int stride = blockDim.x * gridDim.x;
  for (int i = idx; i < n; i += stride) {
    unsigned char val = input[i];
    atomicAdd(&s_hist[val], 1U);
  }
  __syncthreads();

  // Merge block histogram into global memory
  if (tid < 256) {
    atomicAdd(&output[tid], s_hist[tid]);
  }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
  // Choose grid size based on device properties (cached for repeated calls)
  static int grid = 0;
  if (grid == 0) {
    cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop, 0);
    int numSMs = prop.multiProcessorCount;
    int maxThreadsPerSM = prop.maxThreadsPerMultiProcessor;
    int blockSize = 256;
    grid = (numSMs * maxThreadsPerSM * 2) / blockSize;
    if (grid < 1) grid = 1;
  }

  constexpr int kBlock = 256;
  size_t sharedMemSize = 256 * sizeof(unsigned int);

  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, 256 * sizeof(unsigned int));
    histogram_shared_kernel<<<grid, kBlock, sharedMemSize>>>(input, output, n);
  }
}
