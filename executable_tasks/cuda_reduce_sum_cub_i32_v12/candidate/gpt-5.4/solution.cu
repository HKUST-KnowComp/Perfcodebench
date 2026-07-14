#include "interface.h"

#include <cuda_runtime.h>
#include <stdint.h>

namespace {

static inline int ceil_div(int a, int b) { return (a + b - 1) / b; }

__global__ void reduce_sum_i32_kernel(const int* __restrict__ input,
                                      int* __restrict__ output,
                                      int n) {
  __shared__ int sdata[256];

  const int tid = threadIdx.x;
  const int block_threads = blockDim.x;
  const int grid_stride = block_threads * 2 * gridDim.x;
  int idx = (int)(blockIdx.x * (block_threads * 2) + tid);

  int local = 0;

  while (idx < n) {
    local += input[idx];
    const int idx2 = idx + block_threads;
    if (idx2 < n) local += input[idx2];
    idx += grid_stride;
  }

  sdata[tid] = local;
  __syncthreads();

  if (block_threads >= 256) {
    if (tid < 128) sdata[tid] += sdata[tid + 128];
    __syncthreads();
  }
  if (block_threads >= 128) {
    if (tid < 64) sdata[tid] += sdata[tid + 64];
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

  if (tid == 0) atomicAdd(output, sdata[0]);
}

}  // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
  if (iters <= 0) return;

  constexpr int kBlock = 256;

  int device = 0;
  cudaGetDevice(&device);
  cudaDeviceProp prop;
  cudaGetDeviceProperties(&prop, device);

  int grid;
  if (n <= 0) {
    grid = 1;
  } else {
    const int max_blocks_by_n = ceil_div(n, kBlock * 2);
    const int target = prop.multiProcessorCount * 8;
    grid = max_blocks_by_n < target ? max_blocks_by_n : target;
    if (grid < 1) grid = 1;
  }

  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, sizeof(int));
    reduce_sum_i32_kernel<<<grid, kBlock>>>(input, output, n);
  }
}
