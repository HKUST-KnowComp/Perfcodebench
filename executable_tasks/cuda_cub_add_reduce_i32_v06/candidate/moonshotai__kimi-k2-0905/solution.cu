#include <cub/block/block_reduce.cuh>
#include "interface.h"

namespace {
__global__ void reduce_kernel(const int* in, int* out, int n) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    int val = (tid < n) ? in[tid] : 0;

    typedef cub::BlockReduce<int, 512> BlockReduce;
    __shared__ typename BlockReduce::TempStorage temp;
    val = BlockReduce(temp).Sum(val);
    if (threadIdx.x == 0) out[blockIdx.x] = val;
}

__global__ final_reduce(int* partial, int blocks) {
    __shared__ int smem[512];
    int tid = threadIdx.x;
    int sum = 0;
    for (int i = tid; i < blocks; i += blockDim.x) sum += partial[i];
    smem[tid] = sum;
    __syncthreads();
    for (int off = blockDim.x / 2; off > 0; off >>= 1) {
        if (tid < off) smem[tid] += smem[tid + off];
        __syncthreads();
    }
    if (tid == 0) partial[0] = smem[0];
}
} // anonymous

void add_reduce_i32(const int* input, int* output, int n, int iters) {
    const int threads = 512;
    const int blocks = (n + threads - 1) / threads;
    int *tmp;
    cudaMalloc(&tmp, blocks * sizeof(int));
    for (int i = 0; i < iters; ++i) {
        reduce_kernel<<<blocks, threads>>>(input, tmp, n);
        final_reduce<<<1, threads>>>(tmp, blocks);
        cudaMemcpy(output, tmp, sizeof(int), cudaMemcpyDeviceToDevice);
    }
    cudaFree(tmp);
}