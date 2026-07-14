#include "interface.h"
#include <cuda_runtime.h>

namespace {
__global__ void reduce_block_sum(const int* __restrict__ input, int* out, int n) {
    __shared__ int smem[512];
    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x + tid;
    int val = 0;
    // grid-stride load
    while (idx < n) {
        val += input[idx];
        idx += gridDim.x * blockDim.x;
    }
    smem[tid] = val;
    __syncthreads();
    // warp-level reduction
    for (int offset = warpSize/2; offset > 0; offset /= 2)
        smem[tid] += __shfl_down_sync(0xffffffff, smem[tid], offset);
    // block-level reduction
    if (tid < 32) {
        for (int offset = 32; offset < blockDim.x; offset += 32)
            smem[tid] += smem[tid + offset];
        if (tid == 0) atomicAdd(out, smem[0]);
    }
}
} // namespace

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    const int block = 512;
    const int grid  = (n + block - 1) / block;
    for (int i = 0; i < iters; ++i) {
        cudaMemset(output, 0, sizeof(int));
        reduce_block_sum<<<grid, block>>>(input, output, n);
    }
}