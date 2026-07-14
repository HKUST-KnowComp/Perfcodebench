#include "interface.h"

#include <cuda_runtime.h>

namespace {

template<int BLOCK_SIZE>
__device__ void warp_reduce(volatile int* sdata, int tid) {
  if (BLOCK_SIZE >= 64) sdata[tid] += sdata[tid + 32];
  if (BLOCK_SIZE >= 32) sdata[tid] += sdata[tid + 16];
  if (BLOCK_SIZE >= 16) sdata[tid] += sdata[tid + 8];
  if (BLOCK_SIZE >= 8) sdata[tid] += sdata[tid + 4];
  if (BLOCK_SIZE >= 4) sdata[tid] += sdata[tid + 2];
  if (BLOCK_SIZE >= 2) sdata[tid] += sdata[tid + 1];
}

template<int BLOCK_SIZE>
__global__ void reduce_kernel(const int* input, int* output, int n) {
  __shared__ int sdata[BLOCK_SIZE];
  
  int tid = threadIdx.x;
  int idx = blockIdx.x * (BLOCK_SIZE * 2) + threadIdx.x;
  int grid_size = BLOCK_SIZE * 2 * gridDim.x;
  
  int sum = 0;
  while (idx < n) {
    sum += input[idx];
    if (idx + BLOCK_SIZE < n) {
      sum += input[idx + BLOCK_SIZE];
    }
    idx += grid_size;
  }
  sdata[tid] = sum;
  __syncthreads();
  
  if (BLOCK_SIZE >= 512) { if (tid < 256) { sdata[tid] += sdata[tid + 256]; } __syncthreads(); }
  if (BLOCK_SIZE >= 256) { if (tid < 128) { sdata[tid] += sdata[tid + 128]; } __syncthreads(); }
  if (BLOCK_SIZE >= 128) { if (tid < 64) { sdata[tid] += sdata[tid + 64]; } __syncthreads(); }
  
  if (tid < 32) warp_reduce<BLOCK_SIZE>(sdata, tid);
  
  if (tid == 0) atomicAdd(output, sdata[0]);
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  constexpr int kBlock = 256;
  const int grid = min(1024, (n + kBlock * 2 - 1) / (kBlock * 2));
  
  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, sizeof(int));
    reduce_kernel<kBlock><<<grid, kBlock>>>(input, output, n);
  }
}
