#include "interface.h"
#include <cuda_runtime.h>

namespace {

constexpr int BLOCK_SIZE = 256;
constexpr int WARP_SIZE = 32;

__device__ __forceinline__ float warp_reduce(float val) {
    #pragma unroll
    for (int offset = WARP_SIZE / 2; offset > 0; offset >>= 1) {
        val += __shfl_down_sync(0xffffffff, val, offset);
    }
    return val;
}

__global__ void reduce_kernel(const float* __restrict__ input, float* __restrict__ output, int n) {
    __shared__ float shared[BLOCK_SIZE / WARP_SIZE];
    
    int tid = threadIdx.x;
    int gid = blockIdx.x * BLOCK_SIZE + tid;
    int stride = gridDim.x * BLOCK_SIZE;
    
    float sum = 0.0f;
    for (int i = gid; i < n; i += stride) {
        sum += input[i];
    }
    
    sum = warp_reduce(sum);
    
    int lane = tid % WARP_SIZE;
    int warp_id = tid / WARP_SIZE;
    
    if (lane == 0) {
        shared[warp_id] = sum;
    }
    __syncthreads();
    
    if (warp_id == 0) {
        sum = (tid < BLOCK_SIZE / WARP_SIZE) ? shared[tid] : 0.0f;
        sum = warp_reduce(sum);
        if (tid == 0) {
            atomicAdd(output, sum);
        }
    }
}

__global__ void zero_kernel(float* output) {
    *output = 0.0f;
}

}  // namespace

float reduce_f32(const float* input, int n, int iters) {
    float* d_out = nullptr;
    cudaMalloc(&d_out, sizeof(float));
    
    int num_blocks = (n + BLOCK_SIZE - 1) / BLOCK_SIZE;
    num_blocks = min(num_blocks, 1024);
    
    float result = 0.0f;
    
    for (int iter = 0; iter < iters; ++iter) {
        zero_kernel<<<1, 1>>>(d_out);
        reduce_kernel<<<num_blocks, BLOCK_SIZE>>>(input, d_out, n);
    }
    
    cudaMemcpy(&result, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_out);
    
    return result;
}