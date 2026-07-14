#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int kBlockSize = 256;
constexpr int kItemsPerThread = 8;
constexpr int kBlockWork = kBlockSize * kItemsPerThread;

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
    __shared__ unsigned int hist[256];

    // Initialize shared histogram to zero (coalesced write by blocks of threads)
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        hist[i] = 0;
    }
    __syncthreads();

    int base = blockIdx.x * kBlockWork;
    int tid = threadIdx.x;

    // Process kItemsPerThread elements per thread with coalesced global loads
    #pragma unroll
    for (int i = 0; i < kItemsPerThread; ++i) {
        int idx = base + tid + i * kBlockSize;
        if (idx < n) {
            atomicAdd(&hist[input[idx]], 1U);
        }
    }
    __syncthreads();

    // Flush per-block histogram to global memory
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        unsigned int val = hist[i];
        if (val != 0) {
            atomicAdd(output + i, val);
        }
    }
}

}  // anonymous namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    // Determine grid size: enough blocks to cover all input, but at least 1
    int grid = (n + kBlockWork - 1) / kBlockWork;
    if (grid < 1) grid = 1;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_kernel<<<grid, kBlockSize>>>(input, output, n);
    }
}
