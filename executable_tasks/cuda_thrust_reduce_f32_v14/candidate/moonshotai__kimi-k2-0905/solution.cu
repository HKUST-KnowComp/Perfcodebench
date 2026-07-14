#include "interface.h"
#include <cuda_runtime.h>

static __global__ void reduceBlock1(const float * __restrict__ d_in,
                                    float *d_out,
                                    int n)
{
    extern __shared__ float sh[];
    int tid = threadIdx.x;
    int i  = blockIdx.x * blockDim.x * 2 + tid;
    float v = 0.f;
    if (i < n)                  v = d_in[i];
    if (i + blockDim.x < n)      v += d_in[i+blockDim.x];
    sh[tid] = v;
    __syncthreads();

    // block reduction in shared memory
    for (int s = blockDim.x >> 1; s > 32; s >>= 1) {
        if (tid < s) sh[tid] += sh[tid + s];
        __syncthreads();
    }
    // warp reduction without __syncthreads
    if (tid < 32) {
        volatile float *s = sh;
        s[tid] += s[tid + 32];
        s[tid] += s[tid + 16];
        s[tid] += s[tid +  8];
        s[tid] += s[tid +  4];
        s[tid] += s[tid +  2];
        s[tid] += s[tid +  1];
        if (tid == 0) d_out[blockIdx.x] = s[0];
    }
}

float reduce_f32(const float* input, int n, int iters)
{
    static float *d_out = nullptr;      // keeps buffer across calls
    if (!d_out) { float *tmp; cudaMalloc(&tmp, sizeof(float)); d_out = tmp; }

    const int BS = 512;
    const int blocks = (n + BS*2 - 1) / (BS*2);
    size_t  shmem = BS * sizeof(float);

    for (int iter = 0; iter < iters; ++iter) {
        reduceBlock1<<<blocks, BS, shmem>>>(input, d_out, n);
        if (iter == iters-1) break;                        // avoid sync overhead for inner iters
        cudaDeviceSynchronize();
    }
    cudaDeviceSynchronize();
    float h;
    cudaMemcpy(&h, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    return h;
}