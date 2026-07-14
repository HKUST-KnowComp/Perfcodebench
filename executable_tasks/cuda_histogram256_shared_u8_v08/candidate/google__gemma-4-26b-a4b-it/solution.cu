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

    // Each thread processes multiple elements using grid-stride loop
    // We use a local register-based accumulation if possible, but for 256 bins,
    // shared memory is the most practical approach for a single kernel pass.
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    // To further optimize, we can use a small local cache in registers if the bin count was smaller,
    // but for 256 bins, we rely on shared memory.
    for (int i = idx; i < n; i += stride) {
        unsigned char val = input[i];
        // We use atomicAdd on shared memory. 
        // Note: atomicAdd on shared memory is very fast on modern architectures.
        atomicAdd(&local_hist[val], 1U);
    }

    __syncthreads();

    // Aggregate local histogram into global memory
    // Each thread handles one bin to minimize global atomic operations
    if (tid < 256) {
        atomicAdd(&output[tid], local_hist[tid]);
    }
}

} // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    // Tuning parameters
    // 256 threads per block allows us to map 1:1 with the 256 bins for the final reduction
    constexpr int kBlock = 256;
    // Calculate grid size to saturate the GPU while keeping enough blocks for the shared memory pattern
    // A grid size of 256-512 is usually a sweet spot for these types of kernels.
    const int kGrid = 256;

    for (int iter = 0; iter < iters; ++iter) {
        // Reset output
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        
        // Launch kernel
        histogram_shared_kernel<<<kGrid, kBlock>>>(input, output, n);
    }
}