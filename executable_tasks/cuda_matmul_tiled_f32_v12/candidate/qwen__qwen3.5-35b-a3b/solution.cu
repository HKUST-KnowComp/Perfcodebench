#include "interface.h"
#include <cuda_runtime.h>

namespace {

__global__ void matmul_tiled_kernel(const float* a, const float* b, float* c, int n) {
  const int tile_size = 16;
  __shared__ float as[tile_size][tile_size];
  __shared__ float bs[tile_size][tile_size];
  
  const int row = blockIdx.y * tile_size + threadIdx.y;
  const int col = blockIdx.x * tile_size + threadIdx.x;
  
  float sum = 0.0f;
  
  int num_tiles = (n + tile_size - 1) / tile_size;
  
  for (int t = 0; t < num_tiles; ++t) {
    // Load tile of A into shared memory
    int a_row = row;
    int a_col = t * tile_size + threadIdx.x;
    as[threadIdx.y][threadIdx.x] = (a_row < n && a_col < n) 
                                   ? a[a_row * n + a_col] 
                                   : 0.0f;
    
    // Load tile of B into shared memory
    int b_row = t * tile_size + threadIdx.y;
    int b_col = col;
    bs[threadIdx.y][threadIdx.x] = (b_row < n && b_col < n) 
                                   ? b[b_row * n + b_col] 
                                   : 0.0f;
    
    __syncthreads();
    
    // Compute partial dot product for this tile
    for (int k = 0; k < tile_size; ++k) {
      sum += as[threadIdx.y][k] * bs[k][threadIdx.x];
    }
    
    __syncthreads();
  }
  
  // Store result
  if (row < n && col < n) {
    c[row * n + col] = sum;
  }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  const dim3 block(16, 16);
  const dim3 grid((n + block.x - 1) / block.x, (n + block.y - 1) / block.y);
  
  for (int iter = 0; iter < iters; ++iter) {
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
  }
}