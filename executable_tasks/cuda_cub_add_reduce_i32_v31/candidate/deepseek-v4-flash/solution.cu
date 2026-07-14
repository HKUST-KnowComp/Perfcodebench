#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_kernel(const int* input, int* output, int n) {
    unsigned int sum = 0;
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    // Grid-stride loop: each thread accumulates as many elements as needed
    for (; tid < n; tid += stride) {
        sum += static_cast<unsigned int>(input[tid]);
    }

    // Shared memory for block-level reduction
    __shared__ unsigned int sdata[256];
    sdata[threadIdx.x] = sum;
    __syncthreads();

    // Tree reduction in shared memory (for sizes > 32)
    for (unsigned int s = blockDim.x / 2; s > 32; s >>= 1) {
        if (threadIdx.x < s) {
            sdata[threadIdx.x] += sdata[threadIdx.x + s];
        }
        __syncthreads();
    }

    // Final reduction for the remaining 32 (or fewer) elements using warp shuffle
    if (threadIdx.x < 32) {
        unsigned int val = sdata[threadIdx.x];
        #pragma unroll
        for (int offset = 16; offset >= 1; offset >>= 1) {
            val += __shfl_down_sync(0xFFFFFFFF, val, offset);
        }
        if (threadIdx.x == 0) {
            atomicAdd(output, static_cast<int>(val));
        }
    }
}

} // anonymous namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(output, 0, sizeof(int));
        reduce_kernel<<<1024, 256>>>(input, output, n);
    }
}
