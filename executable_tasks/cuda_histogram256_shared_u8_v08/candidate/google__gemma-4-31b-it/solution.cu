#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
    // Shared memory for 256 bins (256 * 4 bytes = 1KB)
    __shared__ unsigned int s_hist[256];

    // Initialize shared memory
    int tid = threadIdx.x;
    if (tid < 256) {
        s_hist[tid] = 0;
    }
    __syncthreads();

    // Grid-stride loop to process input
    int start = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    for (int idx = start; idx < n; idx += stride) {
        atomicAdd(&s_hist[input[idx]], 1);
    }
    __syncthreads();

    // Aggregate shared memory results into global memory
    if (tid < 256) {
        atomicAdd(&output[tid], s_hist[tid]);
    }
}

} // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    // Use a reasonable number of blocks to saturate the GPU
    // 256 threads per block is a good balance for shared memory and occupancy
    const int kBlock = 256;
    const int kGrid = 256;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, 256 * sizeof(unsigned int), 0);
        histogram_shared_kernel<<<kGrid, kBlock>>>(input, output, n);
    }
    cudaDeviceSynchronize();
}