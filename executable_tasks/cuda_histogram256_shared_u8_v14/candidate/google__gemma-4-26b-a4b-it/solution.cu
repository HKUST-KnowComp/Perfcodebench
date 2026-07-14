#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void histogram256_shared_kernel(const unsigned char* __restrict__ input, unsigned int* __restrict__ output, int n) {
    // Each block has its own local histogram in shared memory
    __shared__ unsigned int local_hist[256];

    // Initialize shared memory
    int tid = threadIdx.x;
    if (tid < 256) {
        local_hist[tid] = 0;
    }
    __syncthreads();

    // Each thread processes multiple elements to maximize occupancy and hide latency
    // We use a local array in registers/L1 if possible, but for 256 bins, 
    // shared memory is the most reliable way to avoid massive register pressure.
    // However, to avoid bank conflicts and high shared memory usage, 
    // we'll use a hybrid approach: threads accumulate in shared memory.
    
    // To minimize bank conflicts, we can't easily use a single shared array for all threads 
    // if they all hit the same bin. But for a histogram, contention is inevitable.
    // We use atomicAdd on shared memory which is very fast on modern architectures.
    
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    for (int i = idx; i < n; i += stride) {
        unsigned char val = input[i];
        atomicAdd(&local_hist[val], 1U);
    }

    __syncthreads();

    // Merge local histogram into global output
    // We use a single thread per bin to perform the global atomic add
    if (tid < 256) {
        atomicAdd(&output[tid], local_hist[tid]);
    }
}

// A more optimized version using a per-thread local cache to reduce shared memory contention
__global__ void histogram256_optimized_kernel(const unsigned char* __restrict__ input, unsigned int* __restrict__ output, int n) {
    // Using a smaller local cache per thread to reduce shared memory pressure
    // and then merging to shared memory.
    // However, for 256 bins, the simplest high-performance way is often 
    // block-level shared memory with atomicAdd.
    
    __shared__ unsigned int s_hist[256];
    
    int tid = threadIdx.x;
    if (tid < 256) s_hist[tid] = 0;
    __syncthreads();

    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    // Process data
    for (int i = idx; i < n; i += stride) {
        atomicAdd(&s_hist[input[i]], 1U);
    }
    __syncthreads();

    // Reduction to global
    // To avoid all threads hitting the same global memory, we distribute the work.
    // Each thread handles one bin.
    if (tid < 256) {
        atomicAdd(&output[tid], s_hist[tid]);
    }
}

} // namespace

void histogram256_u8(const unsigned char* input, unsigned int* output, int n, int iters) {
    // Tuning parameters
    // 256 threads per block is good for shared memory usage (256 * 4 = 1KB)
    // We want enough blocks to saturate the GPU but not so many that we hit global contention too hard.
    // However, the bottleneck here is the atomicAdd to global memory in the baseline.
    // Our new approach moves the contention to shared memory.
    
    const int threadsPerBlock = 256;
    // Calculate grid size to cover n elements
    // We use a fixed grid size to keep the kernel launch overhead consistent if needed,
    // but for varying n, we should scale it.
    // For the purpose of this task, we'll use a large enough grid.
    const int blocksPerGrid = 512; 

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, 256 * sizeof(unsigned int));
        // We use the optimized kernel
        histogram256_optimized_kernel<<<blocksPerGrid, threadsPerBlock>>>(input, output, n);
    }
}
