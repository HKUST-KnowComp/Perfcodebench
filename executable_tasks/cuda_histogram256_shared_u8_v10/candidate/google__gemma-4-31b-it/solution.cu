#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* input, unsigned int* output, int n) {
    // Use shared memory for local histogram to reduce global atomic contention.
    // Padding the array to 256 + 16 to help avoid bank conflicts during access.
    __shared__ unsigned int s_hist[256 + 16];

    // Initialize shared memory
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        s_hist[i] = 0;
    }
    __syncthreads();

    // Process input data
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    for (int idx = tid; idx < n; idx += stride) {
        s_hist[input[idx]]++;
    }
    __syncthreads();

    // Aggregate local histogram into global output
    for (int i = threadIdx.x; i < 256; i += blockDim.x) {
        atomicAdd(output + i, s_hist[i]);
    }
}

} // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    // Block size of 256 is generally efficient for this workload
    const int kBlock = 256;
    // Grid size chosen to saturate the GPU while keeping shared memory usage reasonable
    const int kGrid = 256;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_kernel<<<kGrid, kBlock>>>(input, output, n);
    }
}