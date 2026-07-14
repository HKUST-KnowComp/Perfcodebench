#include "interface.h"

#include <cuda_runtime.h>

namespace {

// Tuned block size for modern GPUs; 256 threads gives good occupancy and
// enough work per block for a two-level scan.
constexpr int BLOCK_THREADS = 256;

__global__ void block_exclusive_scan_kernel(const int* __restrict__ input,
                                            int* __restrict__ output,
                                            int* __restrict__ block_sums,
                                            int n) {
  __shared__ int shmem[BLOCK_THREADS];

  const int tid = threadIdx.x;
  const int gid = blockIdx.x * blockDim.x + tid;

  int x = 0;
  if (gid < n) x = input[gid];
  shmem[tid] = x;
  __syncthreads();

  // Inclusive scan in shared memory (Hillis-Steele).
  #pragma unroll
  for (int offset = 1; offset < BLOCK_THREADS; offset <<= 1) {
    int val = 0;
    if (tid >= offset) val = shmem[tid - offset];
    __syncthreads();
    shmem[tid] += val;
    __syncthreads();
  }

  if (gid < n) {
    output[gid] = shmem[tid] - x;
  }

  if (tid == blockDim.x - 1) {
    const int block_start = blockIdx.x * blockDim.x;
    const int valid = n - block_start;
    if (valid > 0) {
      const int last = valid >= blockDim.x ? (blockDim.x - 1) : (valid - 1);
      block_sums[blockIdx.x] = shmem[last];
    }
  }
}

__global__ void scan_block_sums_kernel(int* data, int count) {
  if (blockIdx.x != 0 || threadIdx.x != 0) return;

  int running = 0;
  for (int i = 0; i < count; ++i) {
    const int v = data[i];
    data[i] = running;
    running += v;
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

  const int num_blocks = (n + BLOCK_THREADS - 1) / BLOCK_THREADS;

  int* d_block_sums = nullptr;
  if (num_blocks > 1) {
    cudaMalloc(&d_block_sums, static_cast<size_t>(num_blocks) * sizeof(int));
  }

  for (int iter = 0; iter < iters; ++iter) {
    if (num_blocks == 1) {
      block_exclusive_scan_kernel<<<1, BLOCK_THREADS>>>(input, output, output, n);
    } else {
      block_exclusive_scan_kernel<<<num_blocks, BLOCK_THREADS>>>(input, output, d_block_sums, n);
      scan_block_sums_kernel<<<1, 1>>>(d_block_sums, num_blocks);
      add_block_offsets_kernel<<<num_blocks, BLOCK_THREADS>>>(output, d_block_sums, n);
    }
  }

  if (d_block_sums) cudaFree(d_block_sums);
}
