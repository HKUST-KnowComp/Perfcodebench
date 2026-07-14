#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kBins = 256;
constexpr int kBlock = 256;
constexpr int kGrid = 1024;

__global__ __launch_bounds__(kBlock, 2)
void histogram_shared_atomic_kernel(const unsigned char* __restrict__ input,
                                    unsigned int* __restrict__ output,
                                    int n) {
  __shared__ unsigned int shist[kBins];

  // Zero shared histogram.
  for (int i = threadIdx.x; i < kBins; i += blockDim.x) {
    shist[i] = 0;
  }
  __syncthreads();

  const int tid = blockIdx.x * blockDim.x + threadIdx.x;
  const int stride_threads = blockDim.x * gridDim.x;

  // Vectorized main path: each logical iteration processes 4 bytes.
  const int n4 = n >> 2;
  const uchar4* __restrict__ input4 = reinterpret_cast<const uchar4*>(input);
  for (int i = tid; i < n4; i += stride_threads) {
    uchar4 v = input4[i];
    atomicAdd(&shist[v.x], 1U);
    atomicAdd(&shist[v.y], 1U);
    atomicAdd(&shist[v.z], 1U);
    atomicAdd(&shist[v.w], 1U);
  }

  // Tail handling for the final 0..3 bytes.
  const int tail_start = n4 << 2;
  for (int i = tail_start + tid; i < n; i += stride_threads) {
    atomicAdd(&shist[input[i]], 1U);
  }

  __syncthreads();

  // Flush block-local histogram to global histogram.
  for (int i = threadIdx.x; i < kBins; i += blockDim.x) {
    unsigned int v = shist[i];
    if (v) atomicAdd(output + i, v);
  }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
  if (iters <= 0) return;

  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, kBins * sizeof(unsigned int));
    if (n > 0) {
      histogram_shared_atomic_kernel<<<kGrid, kBlock>>>(input, output, n);
    }
  }
}
