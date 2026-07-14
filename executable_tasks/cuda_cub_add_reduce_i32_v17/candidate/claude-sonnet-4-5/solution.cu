#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void block_reduce_kernel(const int* input, int* output, int n) {
  __shared__ int sdata[256];
  
  int tid = threadIdx.x;
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  int gridSize = blockDim.x * gridDim.x;
  
  int sum = 0;
  for (int i = idx; i < n; i += gridSize) {
    sum += input[i];
  }
  
  sdata[tid] = sum;
  __syncthreads();
  
  // Reduce within block using shared memory
  for (int s = blockDim.x / 2; s > 32; s >>= 1) {
    if (tid < s) {
      sdata[tid] += sdata[tid + s];
    }
    __syncthreads();
  }
  
  // Warp-level reduction (no sync needed)
  if (tid < 32) {
    volatile int* vdata = sdata;
    if (blockDim.x >= 64) vdata[tid] += vdata[tid + 32];
    vdata[tid] += vdata[tid + 16];
    vdata[tid] += vdata[tid + 8];
    vdata[tid] += vdata[tid + 4];
    vdata[tid] += vdata[tid + 2];
    vdata[tid] += vdata[tid + 1];
  }
  
  // Single atomic per block
  if (tid == 0) {
    atomicAdd(output, sdata[0]);
  }
}

}  // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, sizeof(int));
    block_reduce_kernel<<<2048, 256>>>(input, output, n);
  }
}
