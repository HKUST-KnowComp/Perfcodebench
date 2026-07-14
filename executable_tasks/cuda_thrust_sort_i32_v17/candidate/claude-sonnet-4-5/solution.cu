#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int RADIX_BITS = 4;
constexpr int RADIX_SIZE = 1 << RADIX_BITS;
constexpr int RADIX_MASK = RADIX_SIZE - 1;
constexpr int NUM_PASSES = 32 / RADIX_BITS;
constexpr int BLOCK_SIZE = 256;
constexpr int ITEMS_PER_THREAD = 4;

__device__ __forceinline__ int extract_digit(int value, int pass) {
    unsigned int uval = __uint_as_int(value ^ 0x80000000);
    return (uval >> (pass * RADIX_BITS)) & RADIX_MASK;
}

__global__ void radix_sort_kernel(const int* __restrict__ input, 
                                   int* __restrict__ output,
                                   int n,
                                   int pass) {
    __shared__ int local_hist[RADIX_SIZE];
    __shared__ int global_offsets[RADIX_SIZE];
    __shared__ int block_data[BLOCK_SIZE * ITEMS_PER_THREAD];
    
    const int tid = threadIdx.x;
    const int lane_id = tid & 31;
    const int warp_id = tid >> 5;
    
    if (tid < RADIX_SIZE) {
        local_hist[tid] = 0;
    }
    __syncthreads();
    
    int thread_data[ITEMS_PER_THREAD];
    int thread_digits[ITEMS_PER_THREAD];
    
    const int base_idx = blockIdx.x * BLOCK_SIZE * ITEMS_PER_THREAD + tid * ITEMS_PER_THREAD;
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; ++i) {
        int idx = base_idx + i;
        if (idx < n) {
            thread_data[i] = input[idx];
            thread_digits[i] = extract_digit(thread_data[i], pass);
            atomicAdd(&local_hist[thread_digits[i]], 1);
        } else {
            thread_data[i] = 0x7FFFFFFF;
            thread_digits[i] = RADIX_SIZE - 1;
        }
    }
    __syncthreads();
    
    if (tid < RADIX_SIZE) {
        int sum = 0;
        for (int b = 0; b < gridDim.x; ++b) {
            if (b == blockIdx.x) {
                global_offsets[tid] = sum;
            }
            if (b == blockIdx.x) {
                sum += local_hist[tid];
            }
        }
    }
    __syncthreads();
    
    if (tid == 0) {
        int scan = 0;
        for (int i = 0; i < RADIX_SIZE; ++i) {
            int temp = local_hist[i];
            local_hist[i] = scan;
            scan += temp;
        }
    }
    __syncthreads();
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; ++i) {
        int idx = base_idx + i;
        if (idx < n) {
            int digit = thread_digits[i];
            int pos = atomicAdd(&local_hist[digit], 1);
            block_data[pos] = thread_data[i];
        }
    }
    __syncthreads();
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; ++i) {
        int local_idx = tid * ITEMS_PER_THREAD + i;
        if (local_idx < BLOCK_SIZE * ITEMS_PER_THREAD) {
            int value = block_data[local_idx];
            int digit = extract_digit(value, pass);
            int global_idx = global_offsets[digit] + local_idx;
            
            int digit_start = (digit == 0) ? 0 : local_hist[digit - 1];
            int offset_in_digit = local_idx - digit_start;
            global_idx = global_offsets[digit] + offset_in_digit;
            
            if (global_idx < n) {
                output[global_idx] = value;
            }
        }
    }
}

__global__ void histogram_kernel(const int* __restrict__ input,
                                  int* __restrict__ histograms,
                                  int n,
                                  int pass) {
    __shared__ int local_hist[RADIX_SIZE];
    
    const int tid = threadIdx.x;
    const int bid = blockIdx.x;
    
    if (tid < RADIX_SIZE) {
        local_hist[tid] = 0;
    }
    __syncthreads();
    
    const int base_idx = bid * BLOCK_SIZE * ITEMS_PER_THREAD + tid * ITEMS_PER_THREAD;
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; ++i) {
        int idx = base_idx + i;
        if (idx < n) {
            int value = input[idx];
            int digit = extract_digit(value, pass);
            atomicAdd(&local_hist[digit], 1);
        }
    }
    __syncthreads();
    
    if (tid < RADIX_SIZE) {
        histograms[bid * RADIX_SIZE + tid] = local_hist[tid];
    }
}

__global__ void scan_kernel(int* histograms, int num_blocks) {
    __shared__ int temp[RADIX_SIZE];
    
    int tid = threadIdx.x;
    
    if (tid < RADIX_SIZE) {
        int sum = 0;
        for (int b = 0; b < num_blocks; ++b) {
            int val = histograms[b * RADIX_SIZE + tid];
            histograms[b * RADIX_SIZE + tid] = sum;
            sum += val;
        }
    }
}

__global__ void reorder_kernel(const int* __restrict__ input,
                                int* __restrict__ output,
                                const int* __restrict__ histograms,
                                int n,
                                int pass) {
    __shared__ int local_offsets[RADIX_SIZE];
    __shared__ int block_data[BLOCK_SIZE * ITEMS_PER_THREAD];
    __shared__ int block_digits[BLOCK_SIZE * ITEMS_PER_THREAD];
    
    const int tid = threadIdx.x;
    const int bid = blockIdx.x;
    
    if (tid < RADIX_SIZE) {
        local_offsets[tid] = histograms[bid * RADIX_SIZE + tid];
    }
    __syncthreads();
    
    const int base_idx = bid * BLOCK_SIZE * ITEMS_PER_THREAD + tid * ITEMS_PER_THREAD;
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; ++i) {
        int idx = base_idx + i;
        int local_idx = tid * ITEMS_PER_THREAD + i;
        if (idx < n) {
            int value = input[idx];
            block_data[local_idx] = value;
            block_digits[local_idx] = extract_digit(value, pass);
        }
    }
    __syncthreads();
    
    #pragma unroll
    for (int i = 0; i < ITEMS_PER_THREAD; ++i) {
        int local_idx = tid * ITEMS_PER_THREAD + i;
        int idx = base_idx + i;
        if (idx < n) {
            int value = block_data[local_idx];
            int digit = block_digits[local_idx];
            int pos = atomicAdd(&local_offsets[digit], 1);
            output[pos] = value;
        }
    }
}

}  // namespace

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;
    
    const int num_blocks = (n + BLOCK_SIZE * ITEMS_PER_THREAD - 1) / (BLOCK_SIZE * ITEMS_PER_THREAD);
    
    int* histograms;
    cudaMalloc(&histograms, num_blocks * RADIX_SIZE * sizeof(int));
    
    int* temp;
    cudaMalloc(&temp, n * sizeof(int));
    
    for (int iter = 0; iter < iters; ++iter) {
        const int* src = input;
        int* dst = (NUM_PASSES % 2 == 0) ? output : temp;
        
        for (int pass = 0; pass < NUM_PASSES; ++pass) {
            histogram_kernel<<<num_blocks, BLOCK_SIZE>>>(src, histograms, n, pass);
            scan_kernel<<<1, RADIX_SIZE>>>(histograms, num_blocks);
            reorder_kernel<<<num_blocks, BLOCK_SIZE>>>(src, dst, histograms, n, pass);
            
            int* swap = const_cast<int*>(src);
            src = dst;
            dst = swap;
        }
        
        if (NUM_PASSES % 2 == 1) {
            cudaMemcpy(output, temp, n * sizeof(int), cudaMemcpyDeviceToDevice);
        }
    }
    
    cudaFree(histograms);
    cudaFree(temp);
}
