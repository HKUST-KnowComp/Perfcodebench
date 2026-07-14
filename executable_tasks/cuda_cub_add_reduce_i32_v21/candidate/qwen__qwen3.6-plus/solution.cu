#include "interface.h"
#include <cuda_runtime.h>

namespace {
constexpr int BLOCK_SIZE = 256;

__global__ void reduce_kernel(const int* input, int* output, int n) {
    extern __shared__ int sdata[];
    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x * BLOCK_SIZE * 2 + threadIdx.x;
    unsigned int gridSize = BLOCK_SIZE * 2 * gridDim.x;
    int mySum = 0;

    while (i < n) {
        mySum += input[i];
        if (i + BLOCK_SIZE < n) {
            mySum += input[i + BLOCK_SIZE];
        }
        i += gridSize;
    }

    sdata[tid] = mySum;
    __syncthreads();

    for (unsigned int s = BLOCK_SIZE / 2; s > 32; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }

    if (tid < 32) {
        volatile int* vsmem = sdata;
        vsmem[tid] += vsmem[tid + 32];
        vsmem[tid] += vsmem[tid + 16];
        vsmem[tid] += vsmem[tid + 8];
        vsmem[tid] += vsmem[tid + 4];
        vsmem[tid] += vsmem[tid + 2];
        vsmem[tid] += vsmem[tid + 1];
    }

    if (tid == 0) {
        output[blockIdx.x] = sdata[0];
    }
}
} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) {
        for (int i = 0; i < iters; ++i) {
            cudaMemset(output, 0, sizeof(int));
        }
        return;
    }

    constexpr int MAX_BLOCKS = 4096;
    int* d_temp;
    cudaMalloc(&d_temp, MAX_BLOCKS * sizeof(int));

    for (int iter = 0; iter < iters; ++iter) {
        int current_n = n;
        const int* current_in = input;
        int* current_out = d_temp;
        int blocks = (current_n + BLOCK_SIZE * 2 - 1) / (BLOCK_SIZE * 2);
        if (blocks > MAX_BLOCKS) blocks = MAX_BLOCKS;

        reduce_kernel<<<blocks, BLOCK_SIZE, BLOCK_SIZE * sizeof(int)>>>(current_in, current_out, current_n);
        current_n = blocks;
        current_in = current_out;

        while (current_n > 1) {
            blocks = (current_n + BLOCK_SIZE * 2 - 1) / (BLOCK_SIZE * 2);
            if (blocks == 0) blocks = 1;
            if (blocks > MAX_BLOCKS) blocks = MAX_BLOCKS;
            reduce_kernel<<<blocks, BLOCK_SIZE, BLOCK_SIZE * sizeof(int)>>>(current_in, current_out, current_n);
            current_n = blocks;
            current_in = current_out;
        }
        cudaMemcpy(output, current_out, sizeof(int), cudaMemcpyDeviceToDevice);
    }
    cudaFree(d_temp);
}