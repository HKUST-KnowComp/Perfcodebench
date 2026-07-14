#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_kernel(const int* __restrict__ input, int* __restrict__ output, int n) {
    __shared__ int sdata[8];
    int tid = threadIdx.x;
    int lane = tid & 31;
    int warp_id = tid >> 5;
    
    int sum = 0;
    for (int i = blockIdx.x * blockDim.x + tid; i < n; i += blockDim.x * gridDim.x) {
        sum += input[i];
    }
    
    #pragma unroll
    for (int offset = 16; offset > 0; offset >>= 1) {
        sum += __shfl_down_sync(0xffffffff, sum, offset);
    }
    
    if (lane == 0) {
        sdata[warp_id] = sum;
    }
    __syncthreads();
    
    if (warp_id == 0) {
        int warp_sum = (lane < 8) ? sdata[lane] : 0;
        #pragma unroll
        for (int offset = 4; offset > 0; offset >>= 1) {
            warp_sum += __shfl_down_sync(0xffffffff, warp_sum, offset);
        }
        if (lane == 0) {
            atomicAdd(output, warp_sum);
        }
    }
}

} // namespace

void reduce_sum_i32(const int* input, int* output, int n, int iters) {
    constexpr int kBlock = 256;
    int grid = (n + kBlock - 1) / kBlock;
    if (grid > 256) grid = 256;
    if (grid < 1) grid = 1;
    
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<<<grid, kBlock>>>(input, output, n);
    }
}
