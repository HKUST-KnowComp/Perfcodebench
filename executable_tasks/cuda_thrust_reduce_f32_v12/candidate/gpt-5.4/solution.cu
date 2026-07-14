#include "interface.h"
#include <cuda_runtime.h>

namespace {

__inline__ __device__ float warp_reduce_sum(float v) {
    for (int offset = 16; offset > 0; offset >>= 1) {
        v += __shfl_down_sync(0xffffffff, v, offset);
    }
    return v;
}

__global__ void reduce_f32_kernel(const float* __restrict__ input,
                                  float* __restrict__ output,
                                  int n) {
    float sum = 0.0f;

    const int tid = threadIdx.x;
    const int global_stride = blockDim.x * gridDim.x;
    int idx = blockIdx.x * blockDim.x + tid;

    for (; idx < n; idx += global_stride) {
        sum += input[idx];
    }

    sum = warp_reduce_sum(sum);

    __shared__ float warp_sums[32];
    const int lane = tid & 31;
    const int warp = tid >> 5;

    if (lane == 0) warp_sums[warp] = sum;
    __syncthreads();

    if (warp == 0) {
        const int num_warps = blockDim.x >> 5;
        sum = (lane < num_warps) ? warp_sums[lane] : 0.0f;
        sum = warp_reduce_sum(sum);
        if (lane == 0) atomicAdd(output, sum);
    }
}

}  // namespace

float reduce_f32(const float* input, int n, int iters) {
    if (n <= 0 || iters <= 0) return 0.0f;

    float* d_out = nullptr;
    cudaMalloc(&d_out, sizeof(float));

    int device = 0;
    cudaGetDevice(&device);
    cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop, device);

    constexpr int block_size = 256;
    int grid_size = prop.multiProcessorCount * 4;
    int max_grid = (n + block_size - 1) / block_size;
    if (grid_size > max_grid) grid_size = max_grid;
    if (grid_size < 1) grid_size = 1;

    for (int iter = 0; iter < iters; ++iter) {
        cudaMemset(d_out, 0, sizeof(float));
        reduce_f32_kernel<<<grid_size, block_size>>>(input, d_out, n);
    }

    float actual = 0.0f;
    cudaMemcpy(&actual, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_out);
    return actual;
}
