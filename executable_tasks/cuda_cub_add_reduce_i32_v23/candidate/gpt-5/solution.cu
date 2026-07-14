#include "interface.h"
#include <cuda_runtime.h>

namespace {

__inline__ __device__ unsigned int warpReduceSum(unsigned int val) {
  unsigned mask = 0xffffffffu;
  val += __shfl_down_sync(mask, val, 16);
  val += __shfl_down_sync(mask, val, 8);
  val += __shfl_down_sync(mask, val, 4);
  val += __shfl_down_sync(mask, val, 2);
  val += __shfl_down_sync(mask, val, 1);
  return val;
}

template <int BLOCK_SIZE>
__global__ void block_atomic_reduce_kernel(const int* __restrict__ input, int n, int* __restrict__ output) {
  unsigned int local = 0u;
  int idx = blockIdx.x * BLOCK_SIZE + threadIdx.x;
  int stride = BLOCK_SIZE * gridDim.x;

  for (int i = idx; i < n; i += stride) {
    local += static_cast<unsigned int>(input[i]);
  }

  // Intra-warp reduction
  local = warpReduceSum(local);

  __shared__ unsigned int smem[32];
  int lane = threadIdx.x & 31;
  int wid = threadIdx.x >> 5;
  int numWarps = (BLOCK_SIZE + 31) >> 5;

  if (lane == 0) smem[wid] = local;
  __syncthreads();

  unsigned int blockSum = 0u;
  if (threadIdx.x < 32) {
    if (threadIdx.x < numWarps) blockSum = smem[threadIdx.x];
    blockSum = warpReduceSum(blockSum);
    if (threadIdx.x == 0) {
      atomicAdd(reinterpret_cast<unsigned int*>(output), blockSum);
    }
  }
}

}  // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
  // Handle empty input quickly
  if (n <= 0) {
    for (int iter = 0; iter < iters; ++iter) {
      cudaMemset(output, 0, sizeof(int));
    }
    return;
  }

  int device = 0;
  cudaGetDevice(&device);
  int smCount = 0;
  cudaDeviceGetAttribute(&smCount, cudaDevAttrMultiProcessorCount, device);

  const int BLOCK_SIZE = 256;
  int blocks = (smCount > 0 ? smCount * 8 : 1024);
  int maxBlocks = (n + BLOCK_SIZE - 1) / BLOCK_SIZE;
  if (maxBlocks < blocks) blocks = maxBlocks;
  if (blocks < 1) blocks = 1;

  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, sizeof(int));
    block_atomic_reduce_kernel<BLOCK_SIZE><<<blocks, BLOCK_SIZE>>>(input, n, output);
  }
}
