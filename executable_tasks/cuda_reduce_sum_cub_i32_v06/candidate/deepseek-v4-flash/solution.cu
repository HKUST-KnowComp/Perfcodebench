#include "interface.h"
#include <cuda_runtime.h>
#include <cstdint>

// Device-side int64 accumulator (global symbol)
__device__ int64_t d_total;

// Block reduction kernel using warp shuffle + shared memory
__global__ void reduce_kernel(const int* input, int n) {
    int64_t partial = 0;
    // Grid-stride loop for coalesced loads
    for (int idx = blockIdx.x * blockDim.x + threadIdx.x; idx < n; idx += blockDim.x * gridDim.x) {
        partial += input[idx];
    }

    // Warp-level reduction using shuffle
    int warp_id = threadIdx.x / 32;
    int lane = threadIdx.x & 31;
    // Each warp sums its elements
    for (int offset = 16; offset > 0; offset >>= 1) {
        partial += __shfl_down_sync(0xFFFFFFFF, partial, offset);
    }
    // Write warp sum to shared memory
    __shared__ int64_t warp_sum[32]; // max 32 warps (block size <= 1024)
    if (lane == 0) warp_sum[warp_id] = partial;
    __syncthreads();

    // Second-level reduction: reduce warp sums to one block sum
    if (warp_id == 0) {
        int64_t block_sum = (threadIdx.x < (blockDim.x + 31) / 32) ? warp_sum[threadIdx.x] : 0;
        for (int offset = 16; offset > 0; offset >>= 1) {
            block_sum += __shfl_down_sync(0xFFFFFFFF, block_sum, offset);
        }
        if (threadIdx.x == 0) {
            atomicAdd(reinterpret_cast<unsigned long long*>(&d_total), static_cast<unsigned long long>(block_sum));
        }
    }
}

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    // Static device accumulator pointer (allocated once)
    static int64_t* d_acc = nullptr;
    if (d_acc == nullptr) {
        cudaGetSymbolAddress((void**)&d_acc, "d_total");
        // d_acc now points to the device address of d_total
        // No need to allocate separate memory; use the symbol directly
    }

    // Block size = 256, grid size = 1024 (same as baseline)
    constexpr int kBlock = 256;
    const int grid = 1024;

    for (int iter = 0; iter < iters; ++iter) {
        // Reset accumulator to zero
        cudaMemset(d_acc, 0, sizeof(int64_t));

        // Launch kernel
        reduce_kernel<<<grid, kBlock>>>(input, n);

        // Copy low 32 bits of accumulator to output (truncation matches int32 cast)
        cudaMemcpy(output, d_acc, sizeof(int), cudaMemcpyDeviceToDevice);
    }
}
