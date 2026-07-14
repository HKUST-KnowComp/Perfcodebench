#include "interface.h"
#include <cuda_runtime.h>
constexpr int BLOCK = 512;
__global__ void reduce_level1(const int *in, int *out, int n) {
    __shared__ int buf[BLOCK];
    int tid = threadIdx.x;
    int i   = blockIdx.x * blockDim.x + tid;
    int val = (i < n) ? in[i] : 0;
    buf[tid] = val;
    __syncthreads();
    for (int s = blockDim.x >> 1; s > 0; s >>= 1) {
        if (tid < s) buf[tid] += buf[tid + s];
        __syncthreads();
    }
    if (tid == 0) out[blockIdx.x] = buf[0];
}
__global__ void reduce_level2(const int *in, int *out, int blocks) {
    __shared__ int buf[BLOCK];
    int tid = threadIdx.x;
    int val = 0;
    for (int i = tid; i < blocks; i += blockDim.x) val += in[i];
    buf[tid] = val;
    __syncthreads();
    for (int s = blockDim.x >> 1; s > 0; s >>= 1) {
        if (tid < s) buf[tid] += buf[tid + s];
        __syncthreads();
    }
    if (tid == 0) *out = buf[0];
}
void add_reduce_i32(const int *input, int *output, int n, int iters) {
    static int *d_part = nullptr;
    if (!d_part) {
        int blocks = (n + BLOCK - 1) / BLOCK;
        cudaMalloc(&d_part, blocks * sizeof(int));
    }
    for (int iter = 0; iter < iters; ++iter) {
        int blocks = (n + BLOCK - 1) / BLOCK;
        reduce_level1<<<blocks, BLOCK>>>(input, d_part, n);
        reduce_level2<<<1, BLOCK>>>(d_part, output, blocks);
    }
}