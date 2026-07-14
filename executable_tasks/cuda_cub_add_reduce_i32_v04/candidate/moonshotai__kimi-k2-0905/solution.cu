#include "interface.h"
#include <cuda_runtime.h>
#define BLOCK_SIZE 512

__global__ void reduce_stage1(const int *input, int *partial_sums, int n) {
  extern __shared__ int shmem[];
  int tid = threadIdx.x;
  int idx = blockIdx.x * blockDim.x * 2 + tid;
  // Load & first add
  int val = 0;
  if (idx < n) val = input[idx];
  if (idx + blockDim.x < n) val += input[idx + blockDim.x];
  shmem[tid] = val;
  __syncthreads();
  // Tree reduce in shared memory
  for (int stride = blockDim.x / 2; stride > 0; stride >>= 1) {
    if (tid < stride) shmem[tid] += shmem[tid + stride];
    __syncthreads();
  }
  if (tid == 0) partial_sums[blockIdx.x] = shmem[0];
}

__global__ void reduce_stage2(const int *partial_sums, int *block_count, int *output) {
  extern __shared__ int shmem[];
  int tid = threadIdx.x;
  int val = 0;
  int N = *block_count;
  int stride = (N + blockDim.x - 1) / blockDim.x;
  // Accumulate strided partial sums from stage1
  for (int i = tid; i < N; i += blockDim.x) val += partial_sums[i];
  shmem[tid] = val;
  __syncthreads();
  // Single warp reduction
  for (int offset = warpSize / 2; offset > 0; offset >>= 1)
    shmem[tid] += __shfl_down_sync(0xFFFFFFFF, shmem[tid], offset);
  if (tid == 0) *output = shmem[0];
}

void add_reduce_i32(const int *input, int *output, int n, int iters) {
  int *d_partial, *d_block_count;
  int num_blocks = (n + 2 * BLOCK_SIZE - 1) / (2 * BLOCK_SIZE);
  cudaMalloc(&d_partial, sizeof(int) * num_blocks);
  cudaMalloc(&d_block_count, sizeof(int));
  for (int i = 0; i < iters; ++i) {
    // Stage-1 kernel produces num_blocks partial sums
    reduce_stage1<<<num_blocks, BLOCK_SIZE, BLOCK_SIZE * sizeof(int)>>>(input, d_partial, n);
    cudaMemcpyAsync(d_block_count, &num_blocks, sizeof(int), cudaMemcpyHostToDevice);
    // Stage-2 kernel summarizes exactly to one value
    reduce_stage2<<<1, BLOCK_SIZE, BLOCK_SIZE * sizeof(int)>>>(d_partial, d_block_count, output);
  }
  cudaFree(d_partial);
  cudaFree(d_block_count);
}