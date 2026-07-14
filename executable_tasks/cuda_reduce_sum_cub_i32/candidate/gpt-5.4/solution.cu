#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void reduce_sum_block_kernel(const int* __restrict__ input,
                                        int* __restrict__ block_sums,
                                        int n) {
  extern __shared__ int sdata[];

  const unsigned int tid = threadIdx.x;
  const unsigned int block_threads = blockDim.x;
  const unsigned int grid_stride = block_threads * 2u * gridDim.x;
  unsigned int idx = blockIdx.x * (block_threads * 2u) + tid;

  int sum = 0;
  while (idx < static_cast<unsigned int>(n)) {
    sum += input[idx];
    const unsigned int idx2 = idx + block_threads;
    if (idx2 < static_cast<unsigned int>(n)) sum += input[idx2];
    idx += grid_stride;
  }

  sdata[tid] = sum;
  __syncthreads();

  for (unsigned int s = block_threads >> 1; s > 32; s >>= 1) {
    if (tid < s) sdata[tid] += sdata[tid + s];
    __syncthreads();
  }

  if (tid < 32) {
    volatile int* vsmem = sdata;
    if (block_threads >= 64) vsmem[tid] += vsmem[tid + 32];
    if (block_threads >= 32) vsmem[tid] += vsmem[tid + 16];
    if (block_threads >= 16) vsmem[tid] += vsmem[tid + 8];
    if (block_threads >= 8) vsmem[tid] += vsmem[tid + 4];
    if (block_threads >= 4) vsmem[tid] += vsmem[tid + 2];
    if (block_threads >= 2) vsmem[tid] += vsmem[tid + 1];
  }

  if (tid == 0) block_sums[blockIdx.x] = sdata[0];
}

__global__ void finalize_sum_kernel(const int* __restrict__ block_sums,
                                    int* __restrict__ output,
                                    int num_blocks) {
  extern __shared__ int sdata[];

  const unsigned int tid = threadIdx.x;
  int sum = 0;

  for (unsigned int i = tid; i < static_cast<unsigned int>(num_blocks); i += blockDim.x) {
    sum += block_sums[i];
  }

  sdata[tid] = sum;
  __syncthreads();

  for (unsigned int s = blockDim.x >> 1; s > 32; s >>= 1) {
    if (tid < s) sdata[tid] += sdata[tid + s];
    __syncthreads();
  }

  if (tid < 32) {
    volatile int* vsmem = sdata;
    if (blockDim.x >= 64) vsmem[tid] += vsmem[tid + 32];
    if (blockDim.x >= 32) vsmem[tid] += vsmem[tid + 16];
    if (blockDim.x >= 16) vsmem[tid] += vsmem[tid + 8];
    if (blockDim.x >= 8) vsmem[tid] += vsmem[tid + 4];
    if (blockDim.x >= 4) vsmem[tid] += vsmem[tid + 2];
    if (blockDim.x >= 2) vsmem[tid] += vsmem[tid + 1];
  }

  if (tid == 0) *output = sdata[0];
}

inline int ceil_div(int a, int b) {
  return (a + b - 1) / b;
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

  constexpr int kBlock = 256;
  constexpr int kFinalBlock = 256;

  int device = 0;
  cudaGetDevice(&device);
  cudaDeviceProp prop;
  cudaGetDeviceProperties(&prop, device);

  int sm_count = prop.multiProcessorCount > 0 ? prop.multiProcessorCount : 1;
  int max_blocks = sm_count * 8;
  int grid = ceil_div(n, kBlock * 2);
  if (grid < 1) grid = 1;
  if (grid > max_blocks) grid = max_blocks;

  int* block_sums = nullptr;
  cudaMalloc(&block_sums, static_cast<size_t>(grid) * sizeof(int));

  const size_t shm_main = static_cast<size_t>(kBlock) * sizeof(int);
  const size_t shm_final = static_cast<size_t>(kFinalBlock) * sizeof(int);

  for (int iter = 0; iter < iters; ++iter) {
    reduce_sum_block_kernel<<<grid, kBlock, shm_main>>>(input, block_sums, n);
    finalize_sum_kernel<<<1, kFinalBlock, shm_final>>>(block_sums, output, grid);
  }

  cudaFree(block_sums);
}
