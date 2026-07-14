#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void matmul_tiled_kernel(const float* a, const float* b, float* c, int n) {
  const int tile_size = 16;
  __shared__ float as[tile_size][tile_size];
  __shared__ float bs[tile_size][tile_size];
  
  const int col = blockIdx.x * tile_size + threadIdx.x;
  const int row = blockIdx.y * tile_size + threadIdx.y;
  
  float sum = 0.0f;
  
  int num_tiles = (n + tile_size - 1) / tile_size;
  
  for (int t = 0; t < num_tiles; ++t) {
    // Load tile from global memory to shared memory
    as[threadIdx.y][threadIdx.x] = (row < n && t * tile_size + threadIdx.x < n) 
                                   ? a[row * n + t * tile_size + threadIdx.x] 
                                   : 0.0f;
    bs[threadIdx.y][threadIdx.x] = (t * tile_size + threadIdx.y < n && col < n) 
                                   ? b[(t * tile_size + threadIdx.y) * n + col] 
                                   : 0.0f;
    
    __syncthreads();
    
    // Compute partial dot product for this tile
    for (int k = 0; k < tile_size; ++k) {
      sum += as[threadIdx.y][k] * bs[k][threadIdx.x];
    }
    
    __syncthreads();
  }
  
  if (row < n && col < n) {
    c[row * n + col] = sum;
  }
}

}  // namespace

void matmul_f32(const float* a, const float* b, float* c, int n, int iters) {
  const int tile_size = 16;
  const dim3 block(tile_size, tile_size);
  const dim3 grid((n + tile_size - 1) / tile_size, (n + tile_size - 1) / tile_size);
  
  for (int iter = 0; iter < iters; ++iter) {
    matmul_tiled_kernel<<<grid, block>>>(a, b, c, n);
  }
}