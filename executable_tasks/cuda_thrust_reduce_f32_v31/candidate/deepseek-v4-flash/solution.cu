#include "interface.h"
#include <cuda_runtime.h>

static __device__ __forceinline__ float warp_reduce(float val) {
    for (int offset = 16; offset > 0; offset >>= 1)
        val += __shfl_xor_sync(0xffffffff, val, offset);
    return val;
}

static __global__ void reduce_kernel(const float* input, float* output, int n) {
    __shared__ float partial[32];  // one per warp, max 32 warps for 256 threads
    int tid = threadIdx.x;
    int wid = tid / 32;
    int lane = tid % 32;
    int idx = blockIdx.x * blockDim.x + tid;
    
    float sum = 0.0f;
    // grid-stride loop
    for (int i = idx; i < n; i += gridDim.x * blockDim.x)
        sum += input[i];
    
    sum = warp_reduce(sum);
    if (lane == 0)
        partial[wid] = sum;
    __syncthreads();
    
    // only first warp does final reduction of all partials
    if (wid == 0) {
        float val = (tid < blockDim.x / 32) ? partial[lane] : 0.0f;
        val = warp_reduce(val);
        if (tid == 0)
            atomicAdd(output, val);
    }
}

float reduce_f32(const float* input, int n, int iters) {
    if (n <= 0) return 0.0f;
    
    float *d_out = nullptr;
    cudaMalloc(&d_out, sizeof(float));
    
    const int blockSize = 256;
    dim3 block(blockSize, 1, 1);
    dim3 grid((n + blockSize - 1) / blockSize, 1, 1);
    
    float result = 0.0f;
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(d_out, 0, sizeof(float));
        reduce_kernel<<<grid, block>>>(input, d_out, n);
        // Implicit sync via cudaMemcpy below
    }
    
    cudaMemcpy(&result, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_out);
    return result;
}
