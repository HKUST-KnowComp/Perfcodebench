#include "interface.h"

#include <cuda_runtime.h>

namespace {

inline int div_up(int a, int b) { return (a + b - 1) / b; }

constexpr int BLOCK_SIZE = 256;

__global__ void block_exclusive_scan_kernel(const int* __restrict__ input,
                                            int* __restrict__ output,
                                            int* __restrict__ block_sums,
                                            int n) {
  __shared__ int sh[BLOCK_SIZE];

  const int gid = blockIdx.x * blockDim.x + threadIdx.x;
  const int tid = threadIdx.x;

  int x = 0;
  if (gid < n) x = input[gid];
  sh[tid] = x;
  __syncthreads();

  for (int offset = 1; offset < blockDim.x; offset <<= 1) {
    int val = 0;
    if (tid >= offset) val = sh[tid - offset];
    __syncthreads();
    if (tid >= offset) sh[tid] += val;
    __syncthreads();
  }

  if (gid < n) {
    output[gid] = (tid == 0) ? 0 : sh[tid - 1];
  }

  if (block_sums && tid == blockDim.x - 1) {
    block_sums[blockIdx.x] = sh[tid];
  }
}

__global__ void add_block_offsets_kernel(int* __restrict__ output,
                                         const int* __restrict__ block_offsets,
                                         int n) {
  const int gid = blockIdx.x * blockDim.x + threadIdx.x;
  if (gid < n) {
    output[gid] += block_offsets[blockIdx.x];
  }
}

}  // namespace

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
  if (n <= 0 || iters <= 0) return;

  const int num_blocks = div_up(n, BLOCK_SIZE);

  int* d_block_sums = nullptr;
  int* d_block_offsets = nullptr;
  int* h_block_sums = nullptr;
  int* h_block_offsets = nullptr;

  if (num_blocks > 1) {
    cudaMalloc(&d_block_sums, static_cast<size_t>(num_blocks) * sizeof(int));
    cudaMalloc(&d_block_offsets, static_cast<size_t>(num_blocks) * sizeof(int));
    cudaMallocHost(&h_block_sums, static_cast<size_t>(num_blocks) * sizeof(int));
    cudaMallocHost(&h_block_offsets, static_cast<size_t>(num_blocks) * sizeof(int));
  }

  for (int iter = 0; iter < iters; ++iter) {
    block_exclusive_scan_kernel<<<num_blocks, BLOCK_SIZE>>>(input, output, d_block_sums, n);

    if (num_blocks > 1) {
      cudaMemcpy(h_block_sums, d_block_sums, static_cast<size_t>(num_blocks) * sizeof(int), cudaMemcpyDeviceToHost);

      int running = 0;
      for (int i = 0; i < num_blocks; ++i) {
        h_block_offsets[i] = running;
        running += h_block_sums[i];
      }

      cudaMemcpy(d_block_offsets, h_block_offsets, static_cast<size_t>(num_blocks) * sizeof(int), cudaMemcpyHostToDevice);
      add_block_offsets_kernel<<<num_blocks, BLOCK_SIZE>>>(output, d_block_offsets, n);
    }
  }

  if (d_block_sums) cudaFree(d_block_sums);
  if (d_block_offsets) cudaFree(d_block_offsets);
  if (h_block_sums) cudaFreeHost(h_block_sums);
  if (h_block_offsets) cudaFreeHost(h_block_offsets);
}
