#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int BLOCK_SIZE = 256;
constexpr int THREADS_PER_BLOCK = 256;

__global__ void histogram_shared_privatized_kernel(const unsigned char* __restrict__ input,
                                                    unsigned int* __restrict__ output,
                                                    int n) {
    // Each thread has its own private histogram in shared memory
    // Layout: shared_hist[threadIdx.x * 256 + bin]
    __shared__ unsigned int shared_hist[THREADS_PER_BLOCK * 256];
    
    const int tid = threadIdx.x;
    const int global_tid = blockIdx.x * blockDim.x + threadIdx.x;
    const int stride = blockDim.x * gridDim.x;
    
    // Initialize private histogram to zero
    unsigned int* my_hist = shared_hist + tid * 256;
    #pragma unroll
    for (int i = 0; i < 256; i += 4) {
        my_hist[i] = 0;
        my_hist[i + 1] = 0;
        my_hist[i + 2] = 0;
        my_hist[i + 3] = 0;
    }
    __syncthreads();
    
    // Process input using vectorized loads (4 bytes at a time)
    const uchar4* input4 = reinterpret_cast<const uchar4*>(input);
    int n4 = n / 4;
    
    for (int idx = global_tid; idx < n4; idx += stride) {
        uchar4 val = input4[idx];
        my_hist[val.x]++;
        my_hist[val.y]++;
        my_hist[val.z]++;
        my_hist[val.w]++;
    }
    
    // Handle remaining elements
    int base = n4 * 4;
    for (int idx = base + global_tid; idx < n; idx += stride) {
        my_hist[input[idx]]++;
    }
    
    __syncthreads();
    
    // Reduce private histograms: each thread handles 1 bin
    // Thread i sums bin i across all private histograms
    unsigned int sum = 0;
    #pragma unroll 8
    for (int t = 0; t < THREADS_PER_BLOCK; t++) {
        sum += shared_hist[t * 256 + tid];
    }
    
    // Atomic add to global output
    if (sum > 0) {
        atomicAdd(output + tid, sum);
    }
}

}  // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    // Use fewer blocks since each block does more work with privatization
    // Shared memory per block: 256 threads * 256 bins * 4 bytes = 256KB
    // This exceeds typical shared memory limits, so we use a different approach
    
    // Alternative: Use sub-histograms with fewer threads having private histograms
    constexpr int BLOCK = 64;  // Reduced block size
    // 64 threads * 256 bins * 4 bytes = 64KB - still too large for most GPUs
    
    // Use a simpler shared memory approach with warp-level privatization
    constexpr int ACTUAL_BLOCK = 256;
    const int grid = 256;  // Fewer blocks, more work per block
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemsetAsync(output, 0, 256 * sizeof(unsigned int));
        histogram_shared_privatized_kernel<<<grid, ACTUAL_BLOCK>>>(input, output, n);
    }
}