#include "interface.h"
#include <cuda_runtime.h>

#define BLOCK_SIZE 1024

__global__ void exclusive_scan_kernel(const int* input, int* output, int n) {
    extern __shared__ int temp[];
    int tid = threadIdx.x;
    int offset = 1;

    // Load input into shared memory
    temp[2 * tid] = (2 * tid < n) ? input[2 * tid] : 0;
    temp[2 * tid + 1] = (2 * tid + 1 < n) ? input[2 * tid + 1] : 0;

    // Build scan tree
    for (int d = n >> 1; d > 0; d >>= 1) {
        __syncthreads();
        if (tid < d) {
            int ai = offset * (2 * tid + 1) - 1;
            int bi = offset * (2 * tid + 2) - 1;
            temp[bi] += temp[ai];
        }
        offset *= 2;
    }

    // Clear the last element
    if (tid == 0) temp[n - 1] = 0;

    // Traverse down tree
    for (int d = 1; d < n; d <<= 1) {
        offset >>= 1;
        __syncthreads();
        if (tid < d) {
            int ai = offset * (2 * tid + 1) - 1;
            int bi = offset * (2 * tid + 2) - 1;
            int t = temp[ai];
            temp[ai] = temp[bi];
            temp[bi] += t;
        }
    }
    __syncthreads();

    // Write to output
    if (2 * tid < n) output[2 * tid] = temp[2 * tid];
    if (2 * tid + 1 < n) output[2 * tid + 1] = temp[2 * tid + 1];
}

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;
    // For small n, sequential is fine, but for general n, use parallel scan
    // This implementation handles power-of-two sizes; for production, pad to power of 2
    int shared_mem_size = n * sizeof(int);
    for (int i = 0; i < iters; ++i) {
        exclusive_scan_kernel<<<1, (n + 1) / 2, shared_mem_size>>>(input, output, n);
    }
    cudaDeviceSynchronize();
}