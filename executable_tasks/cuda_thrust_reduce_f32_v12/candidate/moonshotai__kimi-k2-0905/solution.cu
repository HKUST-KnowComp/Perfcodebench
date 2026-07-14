#include "interface.h"
#include <cuda_runtime.h>
#include <cooperative_groups.h>
namespace cg = cooperative_groups;

__global__ void reduceBlock(const float* __restrict__ in, float* __restrict__ partial, int n) {
    cg::thread_block block = cg::this_thread_block();
    cg::thread_block_tile<32> warp = cg::tiled_partition<32>(block);
    int tid = block.thread_rank();
    int gtid = blockIdx.x * blockDim.x + tid;
    float val = 0.0f;
    for (int i = gtid; i < n; i += blockDim.x * gridDim.x) val += in[i];
    __shared__ float smem[32];
    int lane = tid & 31;
    int wid  = tid >> 5;
    val = cg::reduce(warp, val, cg::plus<float>());
    if (lane == 0) smem[wid] = val;
    block.sync();
    if (wid == 0) {
        val = (tid < (blockDim.x >> 5)) ? smem[lane] : 0.0f;
        val = cg::reduce(warp, val, cg::plus<float>());
        if (tid == 0) partial[blockIdx.x] = val;
    }
}

__global__ void reduceFinal(const float* __restrict__ partial, float* __restrict__ out, int blocks) {
    cg::thread_block block = cg::this_thread_block();
    cg::thread_block_tile<32> warp = cg::tiled_partition<32>(block);
    int tid = block.thread_rank();
    float val = 0.0f;
    for (int i = tid; i < blocks; i += blockDim.x) val += partial[i];
    val = cg::reduce(warp, val, cg::plus<float>());
    if (tid == 0) *out = val;
}

float reduce_f32(const float* input, int n, int iters) {
    const int blockSize = 256;
    const int numBlocks = std::min((n + blockSize - 1) / blockSize, 1024);
    float *d_partial, *d_out;
    cudaMalloc(&d_partial, numBlocks * sizeof(float));
    cudaMalloc(&d_out, sizeof(float));
    for (int i = 0; i < iters; ++i) {
        reduceBlock<<<numBlocks, blockSize>>>(input, d_partial, n);
        reduceFinal<<<1, blockSize>>>(d_partial, d_out, numBlocks);
    }
    float h_out;
    cudaMemcpy(&h_out, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_partial);
    cudaFree(d_out);
    return h_out;
}