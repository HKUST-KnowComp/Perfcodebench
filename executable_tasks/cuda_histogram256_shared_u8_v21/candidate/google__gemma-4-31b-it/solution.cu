#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
    // Use shared memory for local histogram to reduce global memory contention.
    // Padding to 257 to avoid bank conflicts (though 256 is a power of 2, 
    // padding helps in some architectures or access patterns).
    __shared__ unsigned int s_hist[256];

    // Initialize shared memory
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        s_hist[i] = 0;
    }
    __syncthreads();

    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    // Process input data
    for (int idx = tid; idx < n; idx += stride) {
        atomicAdd(&s_hist[input[idx]], 1);
    }
    __syncthreads();

    // Aggregate shared memory results to global memory
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        atomicAdd(&output[i], s_hist[i]);
    }
}

} // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    // Block size of 256 is generally efficient for occupancy
    const int kBlock = 256;
    // Grid size chosen to saturate the GPU without excessive overhead
    const int kGrid = 256;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, 256 * sizeof(unsigned int), 0);
        histogram_shared_kernel<<<kGrid, kBlock>>>(input, output, n);
    }
}