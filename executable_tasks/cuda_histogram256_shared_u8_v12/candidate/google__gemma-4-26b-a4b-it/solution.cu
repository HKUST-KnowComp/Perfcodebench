#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
    // Local histogram in shared memory (256 bins * 4 bytes = 1KB)
    __shared__ unsigned int s_hist[256];

    // Initialize shared memory
    int tid = threadIdx.x;
    if (tid < 256) {
        s_hist[tid] = 0;
    }
    __syncthreads();

    // Each thread processes multiple elements using grid-stride loop
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    while (idx < n) {
        unsigned char val = input[idx];
        // Use atomicAdd on shared memory to handle multiple threads in block hitting same bin
        atomicAdd(&s_hist[val], 1U);
        idx += stride;
    }
    __syncthreads();

    // Write local results to global memory
    // We use a single thread per bin to reduce atomic contention on global memory
    if (tid < 256) {
        atomicAdd(&output[tid], s_hist[tid]);
    }
}

} // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    constexpr int kBlock = 256;
    // A grid size of 256-512 is usually optimal for modern GPUs to saturate SMs
    // without excessive overhead for small/medium N.
    const int kGrid = 256;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_kernel<<<kGrid, kBlock>>>(input, output, n);
    }
}