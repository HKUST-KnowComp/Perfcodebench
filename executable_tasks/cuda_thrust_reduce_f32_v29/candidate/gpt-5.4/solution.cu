#include "interface.h"
#include <cuda_runtime.h>

namespace {

static inline int ceil_div(int a, int b) { return (a + b - 1) / b; }

__global__ void reduce_stage1_kernel(const float* __restrict__ input,
                                     float* __restrict__ partials,
                                     int n) {
    __shared__ float sdata[256];

    const unsigned int tid = threadIdx.x;
    const unsigned int block_size = blockDim.x;
    const unsigned int grid_stride = block_size * 2 * gridDim.x;
    unsigned int i = blockIdx.x * (block_size * 2) + tid;

    float sum = 0.0f;
    while (i < (unsigned int)n) {
        sum += input[i];
        unsigned int j = i + block_size;
        if (j < (unsigned int)n) sum += input[j];
        i += grid_stride;
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

    if (tid == 0) partials[blockIdx.x] = sdata[0];
}

__global__ void reduce_stage2_kernel(const float* __restrict__ partials,
                                     float* __restrict__ output,
                                     int n) {
    __shared__ float sdata[256];

    const unsigned int tid = threadIdx.x;
    float sum = 0.0f;

    for (unsigned int i = tid; i < (unsigned int)n; i += blockDim.x) {
        sum += partials[i];
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

    int device = 0;
    cudaGetDevice(&device);
    cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop, device);

    int max_blocks = prop.multiProcessorCount * 8;
    int blocks = ceil_div(n, block_size * 2);
    if (blocks < 1) blocks = 1;
    if (blocks > max_blocks) blocks = max_blocks;

    float* d_partials = nullptr;
    float* d_out = nullptr;
    cudaMalloc(&d_partials, static_cast<size_t>(blocks) * sizeof(float));
    cudaMalloc(&d_out, sizeof(float));

    for (int iter = 0; iter < iters; ++iter) {
        reduce_stage1_kernel<<<blocks, block_size>>>(input, d_partials, n);
        reduce_stage2_kernel<<<1, block_size>>>(d_partials, d_out, blocks);
    }

    float actual = 0.0f;
    cudaMemcpy(&actual, d_out, sizeof(float), cudaMemcpyDeviceToHost);

    cudaFree(d_partials);
    cudaFree(d_out);
    return actual;
}
