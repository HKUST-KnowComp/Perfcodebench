#include "interface.h"
#include <cub/cub.cuh>
#include <cuda_runtime.h>
#include <cstdint>

namespace {
constexpr int BLOCK_THREADS = 256;

__global__ void block_striped_sum(const int* __restrict__ input,
                                  int* __restrict__ blockSums,
                                  int n) {
  int tid = blockIdx.x * blockDim.x + threadIdx.x;
  int value = 0;
  // block-striped load to maximize coalescing
  for (int i = tid; i < n; i += gridDim.x * blockDim.x) {
    value += input[i];
  }
  // warp-level reduction
  for (int offset = warpSize / 2; offset > 0; offset /= 2)
    value += __shfl_down_sync(0xffffffff, value, offset);
  // write warp 0 result to shared
  __shared__ int warpSums[BLOCK_THREADS / warpSize];
  int lane = threadIdx.x % warpSize;
  int wid  = threadIdx.x / warpSize;
  if (lane == 0) warpSums[wid] = value;
  __syncthreads();
  // final reduction in warp 0
  if (threadIdx.x < warpSize) {
    value = (threadIdx.x < (BLOCK_THREADS / warpSize)) ? warpSums[threadIdx.x] : 0;
    for (int offset = warpSize / 2; offset > 0; offset /= 2)
      value += __shfl_down_sync(0xffffffff, value, offset);
    if (threadIdx.x == 0) blockSums[blockIdx.x] = value;
  }
}
} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
  int* d_blockSums = nullptr;
  int* d_out       = nullptr;
  void* d_temp     = nullptr;
  size_t tempBytes = 0;

  // one block per SM is usually enough occupancy
  int numBlocks = 0;
  cudaOccupancyMaxActiveBlocksPerMultiprocessor(&numBlocks,
                                                 reinterpret_cast<const void*>(block_striped_sum),
                                                 BLOCK_THREADS, 0);
  int device = 0;
  cudaGetDevice(&device);
  int sms = 0;
  cudaDeviceGetAttribute(&sms, cudaDevAttrMultiProcessorCount, device);
  numBlocks = numBlocks * sms;

  // allocate persistent buffers
  cudaMalloc(&d_blockSums, numBlocks * sizeof(int));
  cudaMalloc(&d_out,       sizeof(int));
  // size CUB temp storage
  cub::DeviceReduce::Sum(nullptr, tempBytes, d_blockSums, d_out, numBlocks);
  cudaMalloc(&d_temp, tempBytes);

  for (int iter = 0; iter < iters; ++iter) {
    // stage-1: reduce input to per-block sums
    block_striped_sum<<<numBlocks, BLOCK_THREADS>>>(input, d_blockSums, n);
    // stage-2: reduce block sums to single int
    cub::DeviceReduce::Sum(d_temp, tempBytes, d_blockSums, d_out, numBlocks);
    // copy result to host pointer
    cudaMemcpy(output, d_out, sizeof(int), cudaMemcpyDeviceToHost);
  }

  cudaFree(d_temp);
  cudaFree(d_out);
  cudaFree(d_blockSums);
}