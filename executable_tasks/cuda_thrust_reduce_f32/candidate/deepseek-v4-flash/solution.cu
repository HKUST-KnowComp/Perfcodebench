#include "interface.h"
#include <cuda_runtime.h>

namespace {

// First kernel: each block reduces a portion of the input using shared memory tree reduction.
__global__ void partial_reduce_kernel(const float* input, float* partial, int n) {
    extern __shared__ float sdata[];
    unsigned int tid = threadIdx.x;
    unsigned int idx = blockIdx.x * blockDim.x + tid;
    float sum = 0.0f;
    // Grid-stride loop to cover all elements
    while (idx < n) {
        sum += input[idx];
        idx += gridDim.x * blockDim.x;
    }
    sdata[tid] = sum;
    __syncthreads();
    // Tree reduction in shared memory
    for (unsigned int s = blockDim.x / 2; s > 0; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }
    if (tid == 0) {
        partial[blockIdx.x] = sdata[0];
    }
}

// Second kernel: reduce the array of partial sums to a single value.
__global__ void total_reduce_kernel(const float* partial, float* result, int num_blocks) {
    extern __shared__ float sdata[];
    unsigned int tid = threadIdx.x;
    unsigned int idx = tid;
    float sum = 0.0f;
    while (idx < num_blocks) {
        sum += partial[idx];
        idx += blockDim.x;
    }
    sdata[tid] = sum;
    __syncthreads();
    for (unsigned int s = blockDim.x / 2; s > 0; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }
    if (tid == 0) {
        *result = sdata[0];
    }
}

} // anonymous namespace

float reduce_f32(const float* input, int n, int iters) {
    // Handle empty input
    if (n == 0) return 0.0f;

    const int blockSize = 256;
    // Number of blocks: at most 256 to keep partial array small, but at least enough to cover n.
    int numBlocks = (n + blockSize - 1) / blockSize;
    if (numBlocks > 256) numBlocks = 256;

    float *d_partial = nullptr;
    float *d_result  = nullptr;
    cudaMalloc(&d_partial, numBlocks * sizeof(float));
    cudaMalloc(&d_result,  sizeof(float));

    // Perform the reduction iters times
    for (int iter = 0; iter < iters; ++iter) {
        // First kernel: reduce chunks
        partial_reduce_kernel<<<numBlocks, blockSize, blockSize * sizeof(float)>>>(input, d_partial, n);
        // Second kernel: combine block sums
        // If only one block, we can directly use its result
        if (numBlocks == 1) {
            cudaMemcpy(d_result, d_partial, sizeof(float), cudaMemcpyDeviceToDevice);
        } else {
            // Use a single block of size up to 256 to reduce partial sums
            int reduceBlockSize = (numBlocks < blockSize) ? numBlocks : blockSize;
            total_reduce_kernel<<<1, reduceBlockSize, reduceBlockSize * sizeof(float)>>>(
                d_partial, d_result, numBlocks);
        }
    }

    float actual = 0.0f;
    cudaMemcpy(&actual, d_result, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_partial);
    cudaFree(d_result);
    return actual;
}
