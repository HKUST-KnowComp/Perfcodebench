#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kBins = 256;
constexpr int kBlock = 256;

__global__ void histogram256_shared_kernel(const unsigned char* __restrict__ input,
                                           unsigned int* __restrict__ output,
                                           int n) {
  __shared__ unsigned int shist[kBins];

  // Zero shared histogram.
  for (int i = threadIdx.x; i < kBins; i += blockDim.x) {
    shist[i] = 0;
  }
  __syncthreads();

  const int tid = blockIdx.x * blockDim.x + threadIdx.x;
  const int stride = blockDim.x * gridDim.x;

  // Process 4 bytes per iteration when possible to reduce loop/control overhead.
  int idx4 = tid * 4;
  const int stride4 = stride * 4;
  for (; idx4 <= n - 4; idx4 += stride4) {
    unsigned int v = *reinterpret_cast<const unsigned int*>(input + idx4);
    atomicAdd(&shist[(v) & 0xFF], 1U);
    atomicAdd(&shist[(v >> 8) & 0xFF], 1U);
    atomicAdd(&shist[(v >> 16) & 0xFF], 1U);
    atomicAdd(&shist[(v >> 24) & 0xFF], 1U);
  }

  // Tail handling for any remaining bytes assigned to this thread's strided sequence.
  for (int idx = idx4; idx < n; idx += stride) {
    atomicAdd(&shist[input[idx]], 1U);
  }

  __syncthreads();

  // Merge per-block shared histogram into global histogram.
  for (int i = threadIdx.x; i < kBins; i += blockDim.x) {
    unsigned int v = shist[i];
    if (v) atomicAdd(output + i, v);
  }
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
  if (grid > 65535) grid = 65535;

  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, kBins * sizeof(unsigned int));
    histogram256_shared_kernel<<<grid, kBlock>>>(input, output, n);
  }
}
