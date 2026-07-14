#include "interface.h"
#include <cuda_runtime.h>

namespace {

// Warp-privatized shared-memory histogram kernel.
// Each block allocates W (warps per block) copies of 256-bin histograms in shared memory.
// Threads update their warp's histogram using shared-memory atomics, then reduce to global.
__global__ void histogram_shared_warp_kernel(const unsigned char* __restrict__ input,
                                             unsigned int* __restrict__ output,
                                             int n) {
  extern __shared__ unsigned int shist[];  // size: warps * 256

  const int tid = threadIdx.x;
  const int block_threads = blockDim.x;
  const int warps = block_threads >> 5;  // blockDim / 32
  const int warp_id = tid >> 5;          // warp index within block

  // Zero out shared histograms
  for (int i = tid; i < warps * 256; i += block_threads) {
    shist[i] = 0u;
  }
  __syncthreads();

  // Grid-stride loop over input
  const int total_threads = block_threads * gridDim.x;
  for (int idx = blockIdx.x * block_threads + tid; idx < n; idx += total_threads) {
    unsigned int v = static_cast<unsigned int>(input[idx]);
    // Update warp-private histogram
    atomicAdd(&shist[warp_id * 256 + v], 1u);
  }
  __syncthreads();

  // Reduce per-warp histograms and add to global
  for (int bin = tid; bin < 256; bin += block_threads) {
    unsigned int sum = 0u;
    // Sum over warps
    for (int w = 0; w < warps; ++w) {
      sum += shist[w * 256 + bin];
    }
    if (sum) {
      atomicAdd(&output[bin], sum);
    }
  }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
  if (n <= 0 || iters <= 0) return;

  // Choose launch configuration based on device properties (computed once)
  static int block_dim = 256;
  static int grid_dim = 0;
  static size_t smem_bytes = 0;
  static bool inited = false;
  if (!inited) {
    cudaDeviceProp prop;
    int dev = 0;
    cudaGetDevice(&dev);
    cudaGetDeviceProperties(&prop, dev);

    // Use 256 threads per block (8 warps) -> 8KB shared memory per block
    block_dim = 256;

    // Choose grid as a small multiple of SM count to limit global atomic pressure
    int sm_count = (prop.multiProcessorCount > 0) ? prop.multiProcessorCount : 1;
    int mult = 6;  // heuristic multiplier
    grid_dim = sm_count * mult;

    // Ensure at least 1 block
    if (grid_dim < 1) grid_dim = 1;

    // Dynamic shared memory size: warps * 256 bins * 4 bytes
    smem_bytes = static_cast<size_t>((block_dim >> 5) * 256 * sizeof(unsigned int));

    inited = true;
  }

  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, 256 * sizeof(unsigned int));
    histogram_shared_warp_kernel<<<grid_dim, block_dim, smem_bytes>>>(input, output, n);
  }
}
