#include "interface.h"
#include <cuda_runtime.h>

__global__ void reduce_kernel(const float * __restrict__ in, float * __restrict__ out, int n) {
    extern __shared__ float shmem[];
    int tid = threadIdx.x;
    int i = blockIdx.x * blockDim.x + tid;
    float val = 0.0f;
    // grid-stride load
    while (i < n) {
        val += in[i];
        i += blockDim.x * gridDim.x;
    }
    // warp-shuffle reduction
    for (int offset = warpSize/2; offset > 0; offset /= 2)
        val += __shfl_down_sync(0xffffffff, val, offset);
    // first thread of each warp writes to shared
    int lane = tid & 31;
    int wid  = tid >> 5;
    if (lane == 0) shmem[wid] = val;
    __syncthreads();
    // final warp reduces shared partials
    val = (tid < (blockDim.x >> 5)) ? shmem[tid] : 0.0f;
    if (wid == 0) {
        for (int offset = warpSize/2; offset > 0; offset /= 2)
            val += __shfl_down_sync(0xffffffff, val, offset);
        if (lane == 0) out[blockIdx.x] = val;
    }
}

__global__ void final_reduce(const float *in, float *out, int blocks) {
    extern __shared__ float shmem[];
    float val = 0.0f;
    for (int i = threadIdx.x; i < blocks; i += blockDim.x)
        val += in[i];
    // same warp-shuffle tree
    for (int offset = warpSize/2; offset > 0; offset /= 2)
        val += __shfl_down_sync(0xffffffff, val, offset);
    if (threadIdx.x == 0) *out = val;
}

float reduce_f32(const float *input, int n, int iters) {
    const int warpSize = 32;
    int threads = 256;
    int warps   = threads / warpSize;
    int blocks  = (n + threads - 1) / threads;
    if (blocks > 1024) blocks = 1024;  // keep it simple

    float *d_part = nullptr, *d_out = nullptr;
    cudaMalloc(&d_part, blocks * sizeof(float));
    cudaMalloc(&d_out,  sizeof(float));

    float result = 0.0f;
    for (int iter = 0; iter < iters; ++iter) {
        reduce_kernel<<<blocks, threads, warps * sizeof(float)>>>(input, d_part, n);
        final_reduce<<<1, warps, warps * sizeof(float)>>>(d_part, d_out, blocks);
    }
    cudaMemcpy(&result, d_out, sizeof(float), cudaMemcpyDeviceToHost);

    cudaFree(d_part);
    cudaFree(d_out);
    return result;
}