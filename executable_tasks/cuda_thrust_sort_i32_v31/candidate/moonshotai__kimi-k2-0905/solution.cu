#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_ptr.h>
#include <thrust/sort.h>
#include <cstdint>

namespace {

static int* d_in  = nullptr;
static int* d_out = nullptr;
static int  cached_n = 0;

__global__ void copy_kernel(const int* __restrict__ in, int* __restrict__ out, int n) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    if (tid < n) out[tid] = in[tid];
}

inline void ensure_buffers(int n) {
    if (n > cached_n) {
        cudaFree(d_in);
        cudaFree(d_out);
        cudaMalloc(&d_in , n * sizeof(int));
        cudaMalloc(&d_out, n * sizeof(int));
        cached_n = n;
    }
}

} // anonymous namespace

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    ensure_buffers(n);

    const int block = 256;
    const int grid  = (n + block - 1) / block;

    for (int iter = 0; iter < iters; ++iter) {
        copy_kernel<<<grid, block>>>(input, d_in, n);
        thrust::device_ptr<int> begin = thrust::device_pointer_cast(d_in);
        thrust::device_ptr<int> end   = begin + n;
        thrust::sort(thrust::device, begin, end);
        copy_kernel<<<grid, block>>>(d_in, output, n);
    }
    cudaDeviceSynchronize();
}