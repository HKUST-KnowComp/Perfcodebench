#include "interface.h"
#include <cuda_runtime.h>

namespace {

__device__ __forceinline__ int warp_reduce_sum(int val) {
    for (int offset = 16; offset > 0; offset /= 2) {
        val += __shfl_down_sync(0xFFFFFFFF, val, offset);
    }
    return val;
}

__global__ void reduce_sum_kernel(const int* __restrict__ input, int* output, int n) {
    extern __shared__ int sdata[];
    
    int tid = threadIdx.x;
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    
    int sum = 0;
    // Grid-stride loop to handle n > total threads
    while (i < n) {
        sum += input[i];
        i += stride;
    }

    // Warp reduction
    sum = warp_reduce_sum(sum);

    // Shared memory reduction
    if ((tid % 32) == 0) {
        sdata[tid / 32] = sum;
    }
    __syncthreads();

    // Final reduction in shared memory (one thread per warp)
    if (tid < (blockDim.x / 32)) {
        int val = (tid < (blockDim.x / 32 + 1)) ? sdata[tid] : 0;
        val = warp_reduce_sum(val);
        if (tid == 0) {
            atomicAdd(output, val);
        }
    }
}

// Optimized kernel using a more robust block-level reduction pattern
__global__ void reduce_sum_optimized_kernel(const int* __restrict__ input, int* output, int n) {
    __shared__ int sdata[256];
    int tid = threadIdx.x;
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    int local_sum = 0;
    while (i < n) {
        local_sum += input[i];
        i += stride;
    }

    // Warp shuffle reduction
    for (int offset = 16; offset > 0; offset /= 2) {
        local_sum += __shfl_down_sync(0xFFFFFFFF, local_sum, offset);
    }

    // Store warp results in shared memory
    if ((tid & 31) == 0) {
        sdata[tid >> 5] = local_sum;
    }
    __syncthreads();

    // Final reduction of the shared memory values
    // Only the first warp of the block performs the final reduction
    int final_val = (tid < (blockDim.x / 32)) ? sdata[tid] : 0;
    if (tid < 32) {
        // Re-run shuffle on the shared memory values
        // Note: sdata size is small enough that we can just use a simple loop or shuffle
        // But for simplicity and correctness in a single block:
        int warp_sum = 0;
        // We need to reduce the values stored in sdata
        // Since sdata is small (max 256/32 = 8), we can do it manually or via shuffle
        // Let's use a simple approach for the last few elements
        int val = (tid < (blockDim.x / 32)) ? sdata[tid] : 0;
        // We need to reduce 'val' which is the sum of the warp
        // However, the previous shuffle already reduced the warp. 
        // We just need to reduce the 'sdata' array.
    }
    
    // Let's use a cleaner approach for the block reduction
    // 1. Warp shuffle
    // 2. Store in shared
    // 3. One warp reduces shared
    // 4. AtomicAdd

    // Re-implementing the block reduction logic clearly:
    // (The above was getting messy, let's use the standard pattern)
}

// Final clean version of the kernel
__global__ void reduce_sum_final(const int* __restrict__ input, int* output, int n) {
    __shared__ int sdata[32]; // Max 256 threads / 32 = 8, 32 is safe
    int tid = threadIdx.x;
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    int val = 0;
    while (i < n) {
        val += input[i];
        i += stride;
    }

    // Warp reduction
    for (int offset = 16; offset > 0; offset /= 2) {
        val += __shfl_down_sync(0xFFFFFFFF, val, offset);
    }

    // Write warp sum to shared memory
    if ((tid & 31) == 0) {
        sdata[tid >> 5] = val;
    }
    __syncthreads();

    // Final reduction in the first warp
    if (tid < 32) {
        int warp_val = (tid < (blockDim.x / 32 + (blockDim.x % 32 == 0 ? 0 : 0))) ? sdata[tid] : 0;
        // Wait, the number of elements in sdata is blockDim.x / 32
        // Let's just use a simple loop for the last step to be safe
        int sum = 0;
        if (tid == 0) {
            int num_warps = (blockDim.x + 31) / 32;
            for (int j = 0; j < num_warps; ++j) {
                sum += sdata[j];
            }
            atomicAdd(output, sum);
        }
    }
}

// Actually, the most efficient way for a single-kernel reduction is:
__global__ void reduce_sum_v3(const int* __restrict__ input, int* output, int n) {
    __shared__ int sdata[32];
    int tid = threadIdx.x;
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    int val = 0;
    while (i < n) {
        val += input[i];
        i += stride;
    }

    for (int offset = 16; offset > 0; offset /= 2) {
        val += __shfl_down_sync(0xFFFFFFFF, val, offset);
    }

    if ((tid & 31) == 0) {
        sdata[tid >> 5] = val;
    }
    __syncthreads();

    if (tid == 0) {
        int sum = 0;
        int num_warps = (blockDim.x + 31) / 32;
        for (int j = 0; j < num_warps; ++j) {
            sum += sdata[j];
        }
        atomicAdd(output, sum);
    }
}

} // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    const int kBlock = 256;
    // Use a smaller grid to avoid excessive atomic contention and maximize occupancy
    // A grid size that fits the GPU's SM count is usually better.
    const int kGrid = 128;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        // Using the v3 kernel
        reduce_sum_v3<<<kGrid, kBlock>>>(input, output, n);
    }
}
