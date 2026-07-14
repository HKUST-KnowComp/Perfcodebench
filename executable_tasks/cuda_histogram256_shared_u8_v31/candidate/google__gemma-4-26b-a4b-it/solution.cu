#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram_shared_kernel(const unsigned char* __restrict__ input, unsigned int* output, int n) {
    // Local histogram in shared memory to reduce global atomic contention
    __shared__ unsigned int local_hist[256];

    // Initialize shared memory
    int tid = threadIdx.x;
    if (tid < 256) {
        local_hist[tid] = 0;
    }
    __syncthreads();

    // Each thread processes multiple elements using a grid-stride loop
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    // Use a local register-based cache for the current thread's bin counts if needed,
    // but for 256 bins, shared memory is the standard efficient approach.
    // To further optimize, we use the shared memory as a fast scratchpad.
    
    // We need to handle the case where blockDim.x < 256 for the initialization,
    // but the task uses kBlock = 256, so tid < 256 is always true.

    for (int i = idx; i < n; i += stride) {
        unsigned char val = input[i];
        // We cannot use atomicAdd on shared memory if multiple threads in the same
        // warp access the same bin simultaneously without care, but atomicAdd
        // on shared memory is highly optimized on modern architectures.
        atomicAdd(&local_hist[val], 1U);
    }

    __syncthreads();

    // Aggregate local histogram into global memory
    // Each thread handles one bin to minimize global atomic contention
    for (int i = tid; i < 256; i += blockDim.x) {
        atomicAdd(&output[i], local_hist[i]);
    }
}

} // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    // Using a block size of 256 to match the number of bins for easy shared memory mapping
    constexpr int kBlock = 256;
    // Calculate grid size to saturate the GPU. 1024 blocks * 256 threads = 262,144 threads.
    // For very large N, this is efficient. For small N, it's still fine.
    const int kGrid = 1024;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_kernel<<<kGrid, kBlock>>>(input, output, n);
    }
}