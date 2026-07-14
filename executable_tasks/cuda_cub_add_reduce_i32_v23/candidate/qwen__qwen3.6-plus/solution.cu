#include "interface.h"
#include <cuda_runtime.h>

namespace {
__global__ void reduce_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    __shared__ int sdata[256];
    int tid = threadIdx.x;
    int bid = blockIdx.x;
    int stride = blockDim.x * gridDim.x;
    
    int sum = 0;
    for (int i = bid * blockDim.x + tid; i < n; i += stride) {
        sum += input[i];
    }
    
    // Warp-level reduction using shuffle intrinsics
    #pragma unroll
    for (int offset = 16; offset > 0; offset /= 2) {
        sum += __shfl_down_sync(0xffffffff, sum, offset);
    }
    
    // Store each warp's sum to shared memory
    if (tid % 32 == 0) {
        sdata[tid / 32] = sum;
    }
    __syncthreads();
    
    // Tree reduction of the 8 warp sums in shared memory
    if (tid < 8) {
        int val = sdata[tid];
        if (tid < 4) val += sdata[tid + 4];
        if (tid < 2) val += sdata[tid + 2];
        if (tid < 1) val += sdata[tid + 1];
        if (tid == 0) {
            atomicAdd(output, val);
        }
    }
}
} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<<<1024, 256>>>(input, output, n);
    }
}
