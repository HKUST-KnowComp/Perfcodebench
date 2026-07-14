#include "interface.h"
#include <cuda_runtime.h>

namespace {

__inline__ __device__ float warp_reduce(float val) {
    for (int offset = 16; offset > 0; offset >>= 1) {
        val += __shfl_down_sync(0xffffffff, val, offset);
    }
    return val;
}

__global__ void reduce_kernel(const float* input, float* output, int n) {
    __shared__ float shared[32];
    
    int tid = threadIdx.x;
    int lane = tid & 31;
    int wid = tid >> 5;
    int gid = blockIdx.x * blockDim.x + threadIdx.x;
    int grid_size = blockDim.x * gridDim.x;
    
    float sum = 0.0f;
    for (int i = gid; i < n; i += grid_size) {
        sum += input[i];
    }
    
    sum = warp_reduce(sum);
    
    if (lane == 0) {
        shared[wid] = sum;
    }
    __syncthreads();
    
    if (wid == 0) {
        sum = (tid < (blockDim.x >> 5)) ? shared[tid] : 0.0f;
        sum = warp_reduce(sum);
        if (tid == 0) {
            atomicAdd(output, sum);
        }
    }
}

}  // namespace

float reduce_f32(const float* input, int n, int iters) {
    float* d_out = nullptr;
    cudaMalloc(&d_out, sizeof(float));
    
    int threads = 256;
    int blocks = min((n + threads - 1) / threads, 1024);
    
    float actual = 0.0f;
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(d_out, 0, sizeof(float));
        reduce_kernel<<<blocks, threads>>>(input, d_out, n);
    }
    
    cudaMemcpy(&actual, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_out);
    return actual;
}
