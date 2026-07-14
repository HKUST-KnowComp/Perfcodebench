#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void matmul_naive_kernel(const float* a, const float* b, float* c, int n) {
  const int col = blockIdx.x * blockDim.x + threadIdx.x;
  const int row = blockIdx.y * blockDim.y + threadIdx.y;
  if (row >= n || col >= n) {
    return;
  }
  float sum = 0.0f;
  for (int k = 0; k < n; ++k) {
    sum += a[row * n + k] * b[k * n + col];
  }
  c[row * n + col] = sum;
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  const dim3 block(16, 16);
  const dim3 grid((n + block.x - 1) / block.x, (n + block.y - 1) / block.y);
  for (int iter = 0; iter < iters; ++iter) {
    matmul_naive_kernel<<<grid, block>>>(a, b, c, n);
  }
}
