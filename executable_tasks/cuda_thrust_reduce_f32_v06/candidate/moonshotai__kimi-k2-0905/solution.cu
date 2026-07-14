#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_block_kernel(const float* __restrict__ in,
                                    float* __restrict__ partial,
                                    int n)
{
    __shared__ float shm[1024];
    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x * 4 + tid;

    float sum = 0.0f;
    #pragma unroll 4
    for (int i = 0; i < 4; ++i) {
        int k = idx + i * blockDim.x;
        if (k < n) sum += in[k];
    }

    shm[tid] = sum;
    __syncthreads();

    for (int off = blockDim.x >> 1; off > 0; off >>= 1) {
        if (tid < off) shm[tid] += shm[tid + off];
        __syncthreads();
    }

    if (tid == 0) partial[blockIdx.x] = shm[0];
}

__global__ void reduce_tail_kernel(const float* partial,
                                   float* __restrict__ out,
                                   int blocks)
{
    __shared__ float shm[1024];
    int tid = threadIdx.x;

    float sum = 0.0f;
    for (int i = tid; i < blocks; i += blockDim.x)
        sum += partial[i];

    shm[tid] = sum;
    __syncthreads();

    for (int off = blockDim.x >> 1; off > 0; off >>= 1) {
        if (tid < off) shm[tid] += shm[tid + off];
        __syncthreads();
    }

    if (tid == 0) *out = shm[0];
}

}  // namespace

float reduce_f32(const float* input, int n, int iters)
{
    const int threads = 1024;
    const int items_per_thread = 4;
    const int items_per_block = threads * items_per_thread;
    const int blocks = (n + items_per_block - 1) / items_per_block;

    float* d_partial = nullptr;
    float* d_out = nullptr;
    cudaMalloc(&d_partial, blocks * sizeof(float));
    cudaMalloc(&d_out,    sizeof(float));

    for (int i = 0; i < iters; ++i) {
        reduce_block_kernel<<<blocks, threads>>>(input, d_partial, n);
        reduce_tail_kernel<<<1, 1024>>>(d_partial, d_out, blocks);
    }

    float result;
    cudaMemcpy(&result, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_partial);
    cudaFree(d_out);
    return result;
}