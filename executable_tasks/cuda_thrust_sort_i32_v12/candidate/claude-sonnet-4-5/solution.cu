#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int RADIX_BITS = 4;
constexpr int RADIX_SIZE = 1 << RADIX_BITS;
constexpr int RADIX_MASK = RADIX_SIZE - 1;
constexpr int NUM_PASSES = 32 / RADIX_BITS;
constexpr int BLOCK_SIZE = 256;

__device__ __forceinline__ int extract_digit(int value, int pass) {
    unsigned int uval = __uint_as_int(value ^ 0x80000000);
    return (uval >> (pass * RADIX_BITS)) & RADIX_MASK;
}

__global__ void radix_sort_histogram(const int* input, int* histograms, int n, int pass) {
    __shared__ int shared_hist[RADIX_SIZE];
    
    int tid = threadIdx.x;
    if (tid < RADIX_SIZE) {
        shared_hist[tid] = 0;
    }
    __syncthreads();
    
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = gridDim.x * blockDim.x;
    
    for (int i = idx; i < n; i += stride) {
        int digit = extract_digit(input[i], pass);
        atomicAdd(&shared_hist[digit], 1);
    }
    __syncthreads();
    
    if (tid < RADIX_SIZE) {
        atomicAdd(&histograms[tid], shared_hist[tid]);
    }
}

__global__ void radix_sort_reorder(const int* input, int* output, const int* offsets, int n, int pass) {
    __shared__ int shared_offsets[RADIX_SIZE];
    __shared__ int local_offsets[BLOCK_SIZE];
    
    int tid = threadIdx.x;
    if (tid < RADIX_SIZE) {
        shared_offsets[tid] = offsets[tid];
    }
    __syncthreads();
    
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = gridDim.x * blockDim.x;
    
    for (int i = idx; i < n; i += stride) {
        int value = input[i];
        int digit = extract_digit(value, pass);
        int pos = atomicAdd(&shared_offsets[digit], 1);
        local_offsets[tid] = pos;
        __syncthreads();
        output[local_offsets[tid]] = value;
        __syncthreads();
    }
}

__global__ void prefix_sum(int* data, int size) {
    int tid = threadIdx.x;
    
    for (int stride = 1; stride < size; stride *= 2) {
        int val = 0;
        if (tid >= stride) {
            val = data[tid - stride];
        }
        __syncthreads();
        if (tid >= stride) {
            data[tid] += val;
        }
        __syncthreads();
    }
}

}  // namespace

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;
    
    int* d_temp;
    int* d_histograms;
    int* d_offsets;
    
    cudaMalloc(&d_temp, n * sizeof(int));
    cudaMalloc(&d_histograms, RADIX_SIZE * sizeof(int));
    cudaMalloc(&d_offsets, RADIX_SIZE * sizeof(int));
    
    int num_blocks = (n + BLOCK_SIZE - 1) / BLOCK_SIZE;
    num_blocks = min(num_blocks, 256);
    
    for (int iter = 0; iter < iters; ++iter) {
        const int* src = input;
        int* dst = output;
        
        for (int pass = 0; pass < NUM_PASSES; ++pass) {
            cudaMemset(d_histograms, 0, RADIX_SIZE * sizeof(int));
            
            radix_sort_histogram<<<num_blocks, BLOCK_SIZE>>>(src, d_histograms, n, pass);
            
            cudaMemcpy(d_offsets, d_histograms, RADIX_SIZE * sizeof(int), cudaMemcpyDeviceToDevice);
            prefix_sum<<<1, RADIX_SIZE>>>(d_offsets, RADIX_SIZE);
            
            int* temp_offsets = new int[RADIX_SIZE];
            cudaMemcpy(temp_offsets, d_offsets, RADIX_SIZE * sizeof(int), cudaMemcpyDeviceToHost);
            for (int i = RADIX_SIZE - 1; i > 0; --i) {
                temp_offsets[i] = temp_offsets[i - 1];
            }
            temp_offsets[0] = 0;
            cudaMemcpy(d_offsets, temp_offsets, RADIX_SIZE * sizeof(int), cudaMemcpyHostToDevice);
            delete[] temp_offsets;
            
            radix_sort_reorder<<<num_blocks, BLOCK_SIZE>>>(src, dst, d_offsets, n, pass);
            
            if (pass < NUM_PASSES - 1) {
                int* swap = (int*)src;
                src = dst;
                dst = (src == input) ? output : d_temp;
                if (dst == d_temp && src == output) {
                    swap = d_temp;
                    d_temp = output;
                    dst = swap;
                }
            }
        }
        
        if (iter < iters - 1) {
            cudaMemcpy((void*)input, output, n * sizeof(int), cudaMemcpyDeviceToDevice);
        }
    }
    
    cudaFree(d_temp);
    cudaFree(d_histograms);
    cudaFree(d_offsets);
}
