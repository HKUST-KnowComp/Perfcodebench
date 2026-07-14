#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int RADIX_BITS = 4;
constexpr int RADIX_SIZE = 1 << RADIX_BITS;
constexpr int RADIX_MASK = RADIX_SIZE - 1;
constexpr int NUM_PASSES = 32 / RADIX_BITS;
constexpr int BLOCK_SIZE = 256;

__global__ void radix_sort_histogram(const int* input, int* histograms, int n, int pass) {
    __shared__ int shared_hist[RADIX_SIZE];
    
    int tid = threadIdx.x;
    if (tid < RADIX_SIZE) shared_hist[tid] = 0;
    __syncthreads();
    
    int shift = pass * RADIX_BITS;
    int idx = blockIdx.x * blockDim.x + tid;
    
    if (idx < n) {
        unsigned int val = input[idx];
        val ^= 0x80000000;
        int digit = (val >> shift) & RADIX_MASK;
        atomicAdd(&shared_hist[digit], 1);
    }
    __syncthreads();
    
    if (tid < RADIX_SIZE) {
        histograms[blockIdx.x * RADIX_SIZE + tid] = shared_hist[tid];
    }
}

__global__ void radix_sort_scan(int* histograms, int num_blocks) {
    __shared__ int temp[RADIX_SIZE];
    int tid = threadIdx.x;
    
    if (tid < RADIX_SIZE) {
        int sum = 0;
        for (int i = 0; i < num_blocks; i++) {
            int val = histograms[i * RADIX_SIZE + tid];
            histograms[i * RADIX_SIZE + tid] = sum;
            sum += val;
        }
        temp[tid] = sum;
    }
    __syncthreads();
    
    if (tid == 0) {
        int sum = 0;
        for (int i = 0; i < RADIX_SIZE; i++) {
            int val = temp[i];
            temp[i] = sum;
            sum += val;
        }
        for (int i = 0; i < RADIX_SIZE; i++) {
            for (int j = 0; j < num_blocks; j++) {
                histograms[j * RADIX_SIZE + i] += temp[i];
            }
        }
    }
}

__global__ void radix_sort_reorder(const int* input, int* output, const int* histograms, int n, int pass) {
    __shared__ int shared_offsets[RADIX_SIZE];
    __shared__ int local_hist[RADIX_SIZE];
    
    int tid = threadIdx.x;
    int bid = blockIdx.x;
    
    if (tid < RADIX_SIZE) {
        shared_offsets[tid] = histograms[bid * RADIX_SIZE + tid];
        local_hist[tid] = 0;
    }
    __syncthreads();
    
    int shift = pass * RADIX_BITS;
    int idx = bid * blockDim.x + tid;
    
    int val = 0;
    int digit = 0;
    if (idx < n) {
        val = input[idx];
        unsigned int uval = val;
        uval ^= 0x80000000;
        digit = (uval >> shift) & RADIX_MASK;
    }
    
    __syncthreads();
    
    if (idx < n) {
        int pos = atomicAdd(&local_hist[digit], 1);
        int global_pos = shared_offsets[digit] + pos;
        output[global_pos] = val;
    }
}

}  // namespace

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;
    
    int num_blocks = (n + BLOCK_SIZE - 1) / BLOCK_SIZE;
    int* histograms;
    cudaMalloc(&histograms, num_blocks * RADIX_SIZE * sizeof(int));
    
    int* temp;
    cudaMalloc(&temp, n * sizeof(int));
    
    for (int iter = 0; iter < iters; ++iter) {
        const int* src = input;
        int* dst = output;
        
        for (int pass = 0; pass < NUM_PASSES; ++pass) {
            radix_sort_histogram<<<num_blocks, BLOCK_SIZE>>>(src, histograms, n, pass);
            radix_sort_scan<<<1, RADIX_SIZE>>>(histograms, num_blocks);
            radix_sort_reorder<<<num_blocks, BLOCK_SIZE>>>(src, dst, histograms, n, pass);
            
            if (pass < NUM_PASSES - 1) {
                int* swap = (int*)src;
                src = dst;
                dst = (src == input) ? output : temp;
                if (src == output) dst = temp;
                if (src == temp) dst = output;
            }
        }
        
        if (NUM_PASSES % 2 == 0 && output != dst) {
            cudaMemcpy(output, temp, n * sizeof(int), cudaMemcpyDeviceToDevice);
        }
    }
    
    cudaFree(histograms);
    cudaFree(temp);
}