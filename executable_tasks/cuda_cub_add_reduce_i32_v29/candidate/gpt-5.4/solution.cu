#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_i32_kernel(const int* __restrict__ input,
                                  int* __restrict__ output,
                                  int n) {
  unsigned int tid = threadIdx.x;
  unsigned int block_size = blockDim.x;
  unsigned int grid_size = block_size * 2u * gridDim.x;
  unsigned int idx = blockIdx.x * (block_size * 2u) + tid;

  int sum = 0;

  while (idx < (unsigned int)n) {
    sum += input[idx];
    unsigned int idx2 = idx + block_size;
    if (idx2 < (unsigned int)n) sum += input[idx2];
    idx += grid_size;
  }

  extern __shared__ int sdata[];
  sdata[tid] = sum;
  __syncthreads();

  for (unsigned int s = block_size >> 1; s > 32; s >>= 1) {
    if (tid < s) sdata[tid] += sdata[tid + s];
    __syncthreads();
  }

  if (tid < 32) {
    volatile int* vsmem = sdata;
    if (block_size >= 64) vsmem[tid] += vsmem[tid + 32];
    if (block_size >= 32) vsmem[tid] += vsmem[tid + 16];
    if (block_size >= 16) vsmem[tid] += vsmem[tid + 8];
    if (block_size >= 8) vsmem[tid] += vsmem[tid + 4];
    if (block_size >= 4) vsmem[tid] += vsmem[tid + 2];
    if (block_size >= 2) vsmem[tid] += vsmem[tid + 1];
  }

  if (tid == 0) atomicAdd(output, sdata[0]);
}

}  // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
  if (iters <= 0) return;

  constexpr int threads = 256;
  int blocks = (n + threads * 2 - 1) / (threads * 2);
  if (blocks < 1) blocks = 1;
  if (blocks > 1024) blocks = 1024;

  size_t shmem = threads * sizeof(int);

  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, sizeof(int));
    reduce_i32_kernel<<<blocks, threads, shmem>>>(input, output, n);
  }
}
