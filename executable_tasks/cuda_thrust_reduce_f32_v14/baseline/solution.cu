#include "interface.h"
#include <cuda_runtime.h>
namespace {
__global__ void sequential_reduce_kernel(const float* input, float* output, int n) { if (blockIdx.x != 0 || threadIdx.x != 0) return; float sum = 0.0f; for (int i = 0; i < n; ++i) sum += input[i]; *output = sum; }
}  // namespace
float reduce_f32(const float* input, int n, int iters) { float* d_out = nullptr; cudaMalloc(&d_out, sizeof(float)); float actual = 0.0f; for (int iter = 0; iter < iters; ++iter) { sequential_reduce_kernel<<<1, 1>>>(input, d_out, n); } cudaMemcpy(&actual, d_out, sizeof(float), cudaMemcpyDeviceToHost); cudaFree(d_out); return actual; }
