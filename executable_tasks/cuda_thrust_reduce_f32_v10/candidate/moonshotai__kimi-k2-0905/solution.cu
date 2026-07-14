#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void reduce_kernel(const float* __restrict__ in, float* __restrict__ out, int N, int iters) {
    const int tid = threadIdx.x;
    const int bid = blockIdx.x;
    const int grid = gridDim.x;
    const int idx0 = bid * 128 + tid;

    extern __shared__ float shmem[];

    float sum = 0.0f;
    for (int iter = 0; iter < iters; ++iter) {
        float local = 0.0f;
        #pragma unroll 4
        for (int i = idx0; i < N; i += grid * 128) {
            local += in[i];
        }
        // warp reduction
        for (int offset = 16; offset > 0; offset >>= 1)
            local += __shfl_down_sync(0xffffffff, local, offset);
        if ((tid & 31) == 0) shmem[tid >> 5] = local;
        __syncthreads();

        // single warp final reduction
        if (tid < 4) {
            float t = shmem[tid];
            t += shmem[tid + 4];
            t += shmem[tid + 8];
            t += shmem[tid + 12];
            t += __shfl_xor_sync(0xffffffff, t, 1);
            t += __shfl_xor_sync(0xffffffff, t, 2);
            if (tid == 0) out[bid] = t;
        }
        __syncthreads();
    }
}

__global__ void final_reduce(float* partial, float* result, int blocks) {
    extern __shared__ float sh[];
    float t = 0.0f;
    for (int i = threadIdx.x; i < blocks; i += blockDim.x) t += partial[i];
    sh[threadIdx.x] = t;
    __syncthreads();
    for (int offset = blockDim.x >> 1; offset > 0; offset >>= 1) {
        if (threadIdx.x < offset) sh[threadIdx.x] += sh[threadIdx.x + offset];
        __syncthreads();
    }
    if (threadIdx.x == 0) *result = sh[0];
}

}  // namespace

float reduce_f32(const float* input, int n, int iters) {
    const int blocks = 128;
    float *d_partial = nullptr, *d_out = nullptr;
    cudaMalloc(&d_partial, blocks * sizeof(float));
    cudaMalloc(&d_out, sizeof(float));

    reduce_kernel<<<blocks, 128, 32 * sizeof(float)>>>(input, d_partial, n, iters);
    final_reduce<<<1, 256, 256 * sizeof(float)>>>(d_partial, d_out, blocks);

    float h_out;
    cudaMemcpy(&h_out, d_out, sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(d_partial);
    cudaFree(d_out);
    return h_out;
}