#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kBlockSize = 256;
constexpr int kMaxBlocks = 1024;

__global__ void reduce_stage1_kernel(const int* __restrict__ input,
                                     int* __restrict__ partials,
                                     int n) {
  __shared__ int sdata[kBlockSize];

  const int tid = threadIdx.x;
  const int global_stride = blockDim.x * gridDim.x;
  int idx = blockIdx.x * blockDim.x * 2 + tid;

  int sum = 0;
  while (idx < n) {
    sum += input[idx];
    const int idx2 = idx + blockDim.x;
    if (idx2 < n) sum += input[idx2];
    idx += global_stride * 2;
  }

  sdata[tid] = sum;
  __syncthreads();

  for (int stride = blockDim.x >> 1; stride > 32; stride >>= 1) {
    if (tid < stride) sdata[tid] += sdata[tid + stride];
    __syncthreads();
  }

  if (tid < 32) {
    volatile int* vsmem = sdata;
    vsmem[tid] += vsmem[tid + 32];
    vsmem[tid] += vsmem[tid + 16];
    vsmem[tid] += vsmem[tid + 8];
    vsmem[tid] += vsmem[tid + 4];
    vsmem[tid] += vsmem[tid + 2];
    vsmem[tid] += vsmem[tid + 1];
  }

  if (tid == 0) partials[blockIdx.x] = sdata[0];
}

__global__ void reduce_stage2_kernel(const int* __restrict__ partials,
                                     int* __restrict__ output,
                                     int n) {
  __shared__ int sdata[kBlockSize];

  const int tid = threadIdx.x;
  int sum = 0;

  for (int idx = tid; idx < n; idx += blockDim.x) {
    sum += partials[idx];
  }

  sdata[tid] = sum;
  __syncthreads();

  for (int stride = blockDim.x >> 1; stride > 32; stride >>= 1) {
    if (tid < stride) sdata[tid] += sdata[tid + stride];
    __syncthreads();
  }

  if (tid < 32) {
    volatile int* vsmem = sdata;
    vsmem[tid] += vsmem[tid + 32];
    vsmem[tid] += vsmem[tid + 16];
    vsmem[tid] += vsmem[tid + 8];
    vsmem[tid] += vsmem[tid + 4];
    vsmem[tid] += vsmem[tid + 2];
    vsmem[tid] += vsmem[tid + 1];
  }

  if (tid == 0) *output = sdata[0];
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  if (iters <= 0) return;

  if (n <= 0) {
    for (int iter = 0; iter < iters; ++iter) {
      cudaMemset(output, 0, sizeof(int));
    }
    return;
  }

  int blocks = (n + (kBlockSize * 2 - 1)) / (kBlockSize * 2);
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
