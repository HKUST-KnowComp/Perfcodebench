#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kBins = 256;
constexpr int kBlock = 256;
constexpr int kItemsPerThread = 16;

__device__ __forceinline__ void warp_agg_add(unsigned int* smem, unsigned int bin) {
  unsigned int active = __activemask();
  unsigned int peers = __match_any_sync(active, bin);
  int leader = __ffs((int)peers) - 1;
  int lane = threadIdx.x & 31;
  if (lane == leader) {
    atomicAdd(&smem[bin], (unsigned int)__popc(peers));
  }
}

__global__ void histogram_partial_kernel(const unsigned char* __restrict__ input,
                                         unsigned int* __restrict__ partial,
                                         int n) {
  __shared__ unsigned int smem[kBins];

  for (int i = threadIdx.x; i < kBins; i += blockDim.x) {
    smem[i] = 0;
  }
  __syncthreads();

  const int tid = blockIdx.x * blockDim.x + threadIdx.x;
  const int stride = blockDim.x * gridDim.x;
  int idx = tid * kItemsPerThread;
  const int vec_stride = stride * kItemsPerThread;

  while (idx < n) {
#pragma unroll
    for (int j = 0; j < kItemsPerThread; ++j) {
      int pos = idx + j;
      if (pos < n) {
        unsigned int bin = input[pos];
        warp_agg_add(smem, bin);
      }
    }
    idx += vec_stride;
  }

  __syncthreads();

  unsigned int* block_partial = partial + blockIdx.x * kBins;
  for (int i = threadIdx.x; i < kBins; i += blockDim.x) {
    block_partial[i] = smem[i];
  }
}

__global__ void reduce_partial_kernel(const unsigned int* __restrict__ partial,
                                      unsigned int* __restrict__ output,
                                      int num_blocks) {
  int bin = threadIdx.x;
  if (bin >= kBins) return;

  unsigned int sum = 0;
  const unsigned int* p = partial + bin;
  for (int b = 0; b < num_blocks; ++b) {
    sum += p[b * kBins];
  }
  output[bin] = sum;
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
  if (iters <= 0) return;

  int device = 0;
  cudaGetDevice(&device);
  cudaDeviceProp prop;
  cudaGetDeviceProperties(&prop, device);

  int sm_count = prop.multiProcessorCount > 0 ? prop.multiProcessorCount : 1;
  int grid = sm_count * 8;
  if (grid < 1) grid = 1;
  if (grid > 4096) grid = 4096;

  unsigned int* partial = nullptr;
  cudaMalloc(&partial, (size_t)grid * kBins * sizeof(unsigned int));

  for (int iter = 0; iter < iters; ++iter) {
    histogram_partial_kernel<<<grid, kBlock>>>(input, partial, n);
    reduce_partial_kernel<<<1, kBins>>>(partial, output, grid);
  }

  cudaFree(partial);
}
