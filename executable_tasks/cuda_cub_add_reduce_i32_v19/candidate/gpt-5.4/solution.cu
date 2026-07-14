#include "interface.h"
#include <cuda_runtime.h>

namespace {

static inline int ceil_div_int(int a, int b) {
  return (a + b - 1) / b;
}

__global__ void reduce_stage1_kernel(const int* __restrict__ input,
                                     int* __restrict__ partials,
                                     int n) {
  __shared__ int smem[256];

  const int tid = threadIdx.x;
  const int block_threads = blockDim.x;
  const int grid_stride = block_threads * 2 * gridDim.x;
  int idx = (blockIdx.x * block_threads * 2) + tid;

  int sum = 0;
  while (idx < n) {
    sum += input[idx];
    const int idx2 = idx + block_threads;
    if (idx2 < n) sum += input[idx2];
    idx += grid_stride;
  }

  smem[tid] = sum;
  __syncthreads();

  if (block_threads >= 256) {
    if (tid < 128) smem[tid] += smem[tid + 128];
    __syncthreads();
  }
  if (block_threads >= 128) {
    if (tid < 64) smem[tid] += smem[tid + 64];
    __syncthreads();
  }

  if (tid < 32) {
    volatile int* vsmem = smem;
    if (block_threads >= 64) vsmem[tid] += vsmem[tid + 32];
    if (block_threads >= 32) vsmem[tid] += vsmem[tid + 16];
    if (block_threads >= 16) vsmem[tid] += vsmem[tid + 8];
    if (block_threads >= 8) vsmem[tid] += vsmem[tid + 4];
    if (block_threads >= 4) vsmem[tid] += vsmem[tid + 2];
    if (block_threads >= 2) vsmem[tid] += vsmem[tid + 1];
  }

  if (tid == 0) partials[blockIdx.x] = smem[0];
}

__global__ void reduce_stage2_kernel(const int* __restrict__ partials,
                                     int* __restrict__ output,
                                     int n) {
  __shared__ int smem[256];

  const int tid = threadIdx.x;
  int sum = 0;

  for (int idx = tid; idx < n; idx += blockDim.x) sum += partials[idx];

  smem[tid] = sum;
  __syncthreads();

  if (blockDim.x >= 256) {
    if (tid < 128) smem[tid] += smem[tid + 128];
    __syncthreads();
  }
  if (blockDim.x >= 128) {
    if (tid < 64) smem[tid] += smem[tid + 64];
    __syncthreads();
  }

  if (tid < 32) {
    volatile int* vsmem = smem;
    if (blockDim.x >= 64) vsmem[tid] += vsmem[tid + 32];
    if (blockDim.x >= 32) vsmem[tid] += vsmem[tid + 16];
    if (blockDim.x >= 16) vsmem[tid] += vsmem[tid + 8];
    if (blockDim.x >= 8) vsmem[tid] += vsmem[tid + 4];
    if (blockDim.x >= 4) vsmem[tid] += vsmem[tid + 2];
    if (blockDim.x >= 2) vsmem[tid] += vsmem[tid + 1];
  }

  if (tid == 0) *output = smem[0];
}

}  // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
  if (iters <= 0) return;

  if (n <= 0) {
    for (int iter = 0; iter < iters; ++iter) {
      cudaMemset(output, 0, sizeof(int));
    }
    return;
  }

  constexpr int kBlockSize = 256;
  constexpr int kMaxBlocks = 1024;

  int blocks = ceil_div_int(n, kBlockSize * 2);
  if (blocks < 1) blocks = 1;
  if (blocks > kMaxBlocks) blocks = kMaxBlocks;

  int* partials = nullptr;
  cudaMalloc(&partials, static_cast<size_t>(blocks) * sizeof(int));

  for (int iter = 0; iter < iters; ++iter) {
    reduce_stage1_kernel<<<blocks, kBlockSize>>>(input, partials, n);
    reduce_stage2_kernel<<<1, kBlockSize>>>(partials, output, blocks);
  }

  cudaFree(partials);
}
