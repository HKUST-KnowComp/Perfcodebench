#include "interface.h"
#include <cuda_runtime.h>

static __global__ void reduce_block(const float * __restrict__ in,
                                    float * __restrict__ part,
                                    int n)
{
    __shared__ float smem[256];
    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x + tid;

    float val = 0.f;
    for (int i = idx; i < n; i += gridDim.x * blockDim.x)
        val += in[i];
    smem[tid] = val;
    __syncthreads();

    // warp-level reduction
    for (int s = blockDim.x >> 1; s > 32; s >>= 1) {
        if (tid < s) smem[tid] += smem[tid + s];
        __syncthreads();
    }
    if (tid < 32) {
        val = smem[tid];
        #pragma unroll
        for (int s = 16; s > 0; s >>= 1)
            val += __shfl_xor_sync(0xffffffff, val, s);
        if (tid == 0) part[blockIdx.x] = val;
    }
}

static __global__ void final_reduce(const float * __restrict__ part,
                                    float * __restrict__ out,
                                    int blocks)
{
    __shared__ float smem[256];
    int tid = threadIdx.x;
    float val = 0.f;
    for (int i = tid; i < blocks; i += blockDim.x) val += part[i];
    smem[tid] = val;
    __syncthreads();

    for (int s = blockDim.x >> 1; s > 32; s >>= 1) {
        if (tid < s) smem[tid] += smem[tid + s];
        __syncthreads();
    }
    if (tid < 32) {
        val = smem[tid];
        #pragma unroll
        for (int s = 16; s > 0; s >>= 1)
            val += __shfl_xor_sync(0xffffffff, val, s);
        if (tid == 0) *out = val;
    }
}

float reduce_f32(const float *d_in, int n, int iters)
{
    const int block = 256;
    const int grid  = std::min<int>(1024, (n + block - 1) / block);

    float *d_part = nullptr;
    float *d_out  = nullptr;
    cudaMalloc(&d_part, grid * sizeof(float));
    cudaMalloc(&d_out,  sizeof(float));

    for (int i = 0; i < iters; ++i) {
        reduce_block<<<grid, block>>>(d_in, d_part, n);
        final_reduce<<<1, block>>>(d_part, d_out, grid);
    }

    float h_out;
    cudaMemcpy(&h_out, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_part);
    cudaFree(d_out);
    return h_out;
}