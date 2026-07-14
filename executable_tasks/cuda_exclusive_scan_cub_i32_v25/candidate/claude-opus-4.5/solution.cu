#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int BLOCK_SIZE = 256;
constexpr int ELEMENTS_PER_BLOCK = BLOCK_SIZE * 2;

__global__ void scan_block_kernel(const int* input, int* output, int* block_sums, int n) {
    __shared__ int temp[ELEMENTS_PER_BLOCK];
    
    int tid = threadIdx.x;
    int block_offset = blockIdx.x * ELEMENTS_PER_BLOCK;
    
    // Load input into shared memory
    int ai = tid;
    int bi = tid + BLOCK_SIZE;
    int global_ai = block_offset + ai;
    int global_bi = block_offset + bi;
    
    temp[ai] = (global_ai < n) ? input[global_ai] : 0;
    temp[bi] = (global_bi < n) ? input[global_bi] : 0;
    
    // Up-sweep (reduce) phase
    int offset = 1;
    for (int d = ELEMENTS_PER_BLOCK >> 1; d > 0; d >>= 1) {
        __syncthreads();
        if (tid < d) {
            int ai_idx = offset * (2 * tid + 1) - 1;
            int bi_idx = offset * (2 * tid + 2) - 1;
            temp[bi_idx] += temp[ai_idx];
        }
        offset *= 2;
    }
    
    // Save block sum and clear last element
    if (tid == 0) {
        if (block_sums != nullptr) {
            block_sums[blockIdx.x] = temp[ELEMENTS_PER_BLOCK - 1];
        }
        temp[ELEMENTS_PER_BLOCK - 1] = 0;
    }
    
    // Down-sweep phase
    for (int d = 1; d < ELEMENTS_PER_BLOCK; d *= 2) {
        offset >>= 1;
        __syncthreads();
        if (tid < d) {
            int ai_idx = offset * (2 * tid + 1) - 1;
            int bi_idx = offset * (2 * tid + 2) - 1;
            int t = temp[ai_idx];
            temp[ai_idx] = temp[bi_idx];
            temp[bi_idx] += t;
        }
    }
    __syncthreads();
    
    // Write results to global memory
    if (global_ai < n) output[global_ai] = temp[ai];
    if (global_bi < n) output[global_bi] = temp[bi];
}

__global__ void add_block_sums_kernel(int* output, const int* block_sums, int n) {
    int idx = blockIdx.x * ELEMENTS_PER_BLOCK + threadIdx.x;
    if (blockIdx.x > 0 && idx < n) {
        output[idx] += block_sums[blockIdx.x];
    }
    int idx2 = idx + BLOCK_SIZE;
    if (blockIdx.x > 0 && idx2 < n) {
        output[idx2] += block_sums[blockIdx.x];
    }
}

__global__ void scan_block_sums_kernel(int* block_sums, int num_blocks) {
    // Simple sequential scan for block sums (typically small)
    if (threadIdx.x == 0 && blockIdx.x == 0) {
        int running = 0;
        for (int i = 0; i < num_blocks; ++i) {
            int val = block_sums[i];
            block_sums[i] = running;
            running += val;
        }
    }
}

}  // namespace

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;
    
    int num_blocks = (n + ELEMENTS_PER_BLOCK - 1) / ELEMENTS_PER_BLOCK;
    
    int* block_sums = nullptr;
    if (num_blocks > 1) {
        cudaMalloc(&block_sums, num_blocks * sizeof(int));
    }
    
    for (int iter = 0; iter < iters; ++iter) {
        if (num_blocks == 1) {
            scan_block_kernel<<<1, BLOCK_SIZE>>>(input, output, nullptr, n);
        } else {
            // Phase 1: Scan each block independently, save block sums
            scan_block_kernel<<<num_blocks, BLOCK_SIZE>>>(input, output, block_sums, n);
            
            // Phase 2: Scan block sums
            scan_block_sums_kernel<<<1, 1>>>(block_sums, num_blocks);
            
            // Phase 3: Add scanned block sums to each block
            add_block_sums_kernel<<<num_blocks, BLOCK_SIZE>>>(output, block_sums, n);
        }
    }
    
    if (block_sums != nullptr) {
        cudaFree(block_sums);
    }
}