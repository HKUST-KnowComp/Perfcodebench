#include "interface.h"
#include <cuda_runtime.h>

namespace {

static inline int div_up(int a, int b) { return (a + b - 1) / b; }

__global__ void reduce_stage1_kernel(const int* __restrict__ input,
                                     int* __restrict__ partial,
                                     int n) {
  unsigned int tid = threadIdx.x;
  unsigned int block_size = blockDim.x;
  unsigned int grid_stride = block_size * 2u * gridDim.x;
  unsigned int idx = blockIdx.x * (block_size * 2u) + tid;

  int sum = 0;
  while (idx < (unsigned int)n) {
    sum += input[idx];
    unsigned int idx2 = idx + block_size;
    if (idx2 < (unsigned int)n) sum += input[idx2];
    idx += grid_stride;
  }

  extern __shared__ int sdata[];
  sdata[tid] = sum;
  __syncthreads();

  for (unsigned int s = block_size >> 1; s >= 64; s >>= 1) {
    if (tid < s) sdata[tid] += sdata[tid + s];
    __syncthreads();
    if (s == 64) break;
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

  if (tid == 0) partial[blockIdx.x] = sdata[0];
}

__global__ void reduce_stage2_kernel(const int* __restrict__ partial,
                                     int* __restrict__ output,
                                     int n) {
  unsigned int tid = threadIdx.x;
  int sum = 0;

  for (unsigned int idx = tid; idx < (unsigned int)n; idx += blockDim.x) {
    sum += partial[idx];
  }

  extern __shared__ int sdata[];
  sdata[tid] = sum;
  __syncthreads();

  for (unsigned int s = blockDim.x >> 1; s >= 64; s >>= 1) {
    if (tid < s) sdata[tid] += sdata[tid + s];
    __syncthreads();
    if (s == 64) break;
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

}  // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
  if (iters <= 0) return;

  if (n <= 0) {
    for (int iter = 0; iter < iters; ++iter) {
      cudaMemset(output, 0, sizeof(int));
    }
    return;
  }

  int device = 0;
  cudaGetDevice(&device);
  cudaDeviceProp prop;
  cudaGetDeviceProperties(&prop, device);

  const int block_size = 256;
  int sm_count = prop.multiProcessorCount > 0 ? prop.multiProcessorCount : 1;
  int grid_size = div_up(n, block_size * 2);
  int max_blocks = sm_count * 8;
  if (grid_size > max_blocks) grid_size = max_blocks;
  if (grid_size < 1) grid_size = 1;

  int* partial = nullptr;
  cudaMalloc(&partial, static_cast<size_t>(grid_size) * sizeof(int));

  const int stage1_smem = block_size * (int)sizeof(int);
  int stage2_threads = 256;
  while (stage2_threads > grid_size && stage2_threads > 1) stage2_threads >>= 1;
  if (stage2_threads < 1) stage2_threads = 1;
  const int stage2_smem = stage2_threads * (int)sizeof(int);

  for (int iter = 0; iter < iters; ++iter) {
    reduce_stage1_kernel<<<grid_size, block_size, stage1_smem>>>(input, partial, n);
    reduce_stage2_kernel<<<1, stage2_threads, stage2_smem>>>(partial, output, grid_size);
  }

  cudaFree(partial);
}
