#include "interface.h"
#include <cuda_runtime.h>

namespace {

static inline int ceil_div(int a, int b) { return (a + b - 1) / b; }

__global__ void reduce_stage1_kernel(const float* __restrict__ input,
                                     float* __restrict__ partial,
                                     int n) {
    __shared__ float sdata[256];

    const unsigned int tid = threadIdx.x;
    const unsigned int block_size = blockDim.x;
    unsigned int idx = blockIdx.x * (block_size * 2) + tid;
    const unsigned int grid_stride = gridDim.x * (block_size * 2);

    float sum = 0.0f;
    while (idx < (unsigned int)n) {
        sum += input[idx];
        unsigned int idx2 = idx + block_size;
        if (idx2 < (unsigned int)n) sum += input[idx2];
        idx += grid_stride;
    }

    sdata[tid] = sum;
    __syncthreads();

    if (block_size >= 256) {
        if (tid < 128) sdata[tid] += sdata[tid + 128];
        __syncthreads();
    }
    if (block_size >= 128) {
        if (tid < 64) sdata[tid] += sdata[tid + 64];
        __syncthreads();
    }

    if (tid < 32) {
        volatile float* vsmem = sdata;
        if (block_size >= 64) vsmem[tid] += vsmem[tid + 32];
        if (block_size >= 32) vsmem[tid] += vsmem[tid + 16];
        if (block_size >= 16) vsmem[tid] += vsmem[tid + 8];
        if (block_size >= 8) vsmem[tid] += vsmem[tid + 4];
        if (block_size >= 4) vsmem[tid] += vsmem[tid + 2];
        if (block_size >= 2) vsmem[tid] += vsmem[tid + 1];
    }

    if (tid == 0) partial[blockIdx.x] = sdata[0];
}

__global__ void reduce_final_kernel(const float* __restrict__ partial,
                                    float* __restrict__ output,
                                    int n) {
    __shared__ float sdata[256];

    const unsigned int tid = threadIdx.x;
    float sum = 0.0f;

    for (unsigned int i = tid; i < (unsigned int)n; i += blockDim.x) {
        sum += partial[i];
    }

    sdata[tid] = sum;
    __syncthreads();

    if (blockDim.x >= 256) {
        if (tid < 128) sdata[tid] += sdata[tid + 128];
        __syncthreads();
    }
    if (blockDim.x >= 128) {
        if (tid < 64) sdata[tid] += sdata[tid + 64];
        __syncthreads();
    }

    if (tid < 32) {
        volatile float* vsmem = sdata;
        if (blockDim.x >= 64) vsmem[tid] += vsmem[tid + 32];
        if (blockDim.x >= 32) vsmem[tid] += vsmem[tid + 16];
        if (blockDim.x >= 16) vsmem[tid] += vsmem[tid + 8];
        if (blockDim.x >= 8) vsmem[tid] += vsmem[tid + 4];
        if (blockDim.x >= 4) vsmem[tid] += vsmem[tid + 2];
        if (blockDim.x >= 2) vsmem[tid] += vsmem[tid + 1];
    }

    if (tid == 0) *output = sdata[0];
}

}  // namespace

float reduce_f32(const float* input, int n, int iters) {
    if (n <= 0 || iters <= 0) return 0.0f;

    constexpr int block_size = 256;
    int num_blocks = ceil_div(n, block_size * 2);
    if (num_blocks < 1) num_blocks = 1;

    int device = 0;
    cudaGetDevice(&device);
    cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop, device);
    int max_blocks = prop.multiProcessorCount * 8;
    if (num_blocks > max_blocks) num_blocks = max_blocks;
    if (num_blocks < 1) num_blocks = 1;

    float* d_partial = nullptr;
    float* d_out = nullptr;
    cudaMalloc(&d_partial, static_cast<size_t>(num_blocks) * sizeof(float));
    cudaMalloc(&d_out, sizeof(float));

    for (int iter = 0; iter < iters; ++iter) {
        reduce_stage1_kernel<<<num_blocks, block_size>>>(input, d_partial, n);
        reduce_final_kernel<<<1, block_size>>>(d_partial, d_out, num_blocks);
    }

    float actual = 0.0f;
    cudaMemcpy(&actual, d_out, sizeof(float), cudaMemcpyDeviceToHost);

    cudaFree(d_partial);
    cudaFree(d_out);
    return actual;
}
