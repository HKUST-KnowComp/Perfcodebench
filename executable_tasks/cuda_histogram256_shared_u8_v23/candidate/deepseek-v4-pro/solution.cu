#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_kernel(const unsigned char* input, unsigned int* output, int n) {
  __shared__ unsigned int hist[256];
  
  // Initialize shared histogram to zero
  if (threadIdx.x < 256) {
    hist[threadIdx.x] = 0;
  }
  __syncthreads();
  
  // Grid-stride loop
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  int stride = blockDim.x * gridDim.x;
  for (int i = idx; i < n; i += stride) {
    unsigned char val = input[i];
    atomicAdd(&hist[val], 1);
  }
  
  __syncthreads();
  
  // Accumulate into global memory
  if (threadIdx.x < 256) {
    atomicAdd(&output[threadIdx.x], hist[threadIdx.x]);
  }
}

int compute_optimal_grid_size() {
  int device;
  cudaGetDevice(&device);
  cudaDeviceProp prop;
  cudaGetDeviceProperties(&prop, device);
  int maxActiveBlocks;
  cudaOccupancyMaxActiveBlocksPerMultiprocessor(&maxActiveBlocks, histogram_kernel, 256, 0);
  int gridSize = prop.multiProcessorCount * maxActiveBlocks;
  return (gridSize < 1) ? 1 : gridSize;
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
  const int kBlock = 256;
  static int gridSize = 0;
  if (gridSize == 0) {
    gridSize = compute_optimal_grid_size();
  }
  
  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, 256 * sizeof(unsigned int));
    histogram_kernel<<<gridSize, kBlock>>>(input, output, n);
  }
}
