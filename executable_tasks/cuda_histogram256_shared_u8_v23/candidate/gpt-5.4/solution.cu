#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kBins = 256;
constexpr int kBlock = 256;
constexpr int kWarpsPerBlock = kBlock / 32;
constexpr int kGrid = 256;

struct TempStorage {
  unsigned int* partial = nullptr;
  int capacity_blocks = 0;
};

static TempStorage& temp_storage() {
  static TempStorage s;
  return s;
}

static inline void ensure_partial_capacity(int blocks) {
  TempStorage& s = temp_storage();
  if (s.capacity_blocks >= blocks && s.partial != nullptr) return;
  if (s.partial) cudaFree(s.partial);
  cudaMalloc(&s.partial, static_cast<size_t>(blocks) * kBins * sizeof(unsigned int));
  s.capacity_blocks = blocks;
}

__global__ void histogram_partial_kernel(const unsigned char* __restrict__ input,
                                         unsigned int* __restrict__ partial,
                                         int n) {
  __shared__ unsigned int shist[kWarpsPerBlock][kBins];

  const int tid = threadIdx.x;
  const int lane = tid & 31;
  const int warp = tid >> 5;

  for (int i = tid; i < kWarpsPerBlock * kBins; i += blockDim.x) {
    shist[i >> 8][i & 255] = 0;
  }
  __syncthreads();

  const int global_thread = blockIdx.x * blockDim.x + tid;
  const int stride = blockDim.x * gridDim.x;

  int idx = global_thread;

  for (; idx + 3 * stride < n; idx += 4 * stride) {
    unsigned char v0 = input[idx];
    unsigned char v1 = input[idx + stride];
    unsigned char v2 = input[idx + 2 * stride];
    unsigned char v3 = input[idx + 3 * stride];
    atomicAdd(&shist[warp][v0], 1U);
    atomicAdd(&shist[warp][v1], 1U);
    atomicAdd(&shist[warp][v2], 1U);
    atomicAdd(&shist[warp][v3], 1U);
  }
  for (; idx < n; idx += stride) {
    atomicAdd(&shist[warp][input[idx]], 1U);
  }
  __syncthreads();

  unsigned int* block_partial = partial + static_cast<size_t>(blockIdx.x) * kBins;
  for (int bin = tid; bin < kBins; bin += blockDim.x) {
    unsigned int sum = 0;
#pragma unroll
    for (int w = 0; w < kWarpsPerBlock; ++w) {
      sum += shist[w][bin];
    }
    block_partial[bin] = sum;
  }
}

__global__ void histogram_reduce_kernel(const unsigned int* __restrict__ partial,
                                        unsigned int* __restrict__ output,
                                        int blocks) {
  int bin = threadIdx.x;
  if (bin >= kBins) return;

  unsigned int sum = 0;
  const unsigned int* p = partial + bin;
  for (int b = 0; b < blocks; ++b) {
    sum += p[static_cast<size_t>(b) * kBins];
  }
  output[bin] = sum;
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
  if (iters <= 0) return;

  const int grid = kGrid;
  ensure_partial_capacity(grid);

  for (int iter = 0; iter < iters; ++iter) {
    histogram_partial_kernel<<<grid, kBlock>>>(input, temp_storage().partial, n);
    histogram_reduce_kernel<<<1, kBins>>>(temp_storage().partial, output, grid);
  }
}
