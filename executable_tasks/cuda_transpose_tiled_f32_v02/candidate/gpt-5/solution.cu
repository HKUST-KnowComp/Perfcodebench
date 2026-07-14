#include "interface.h"

#include <cuda_runtime.h>

namespace {

constexpr int kTileDim = 32;
constexpr int kBlockRows = 8;

// Use launch bounds to hint at occupancy and register allocation
__launch_bounds__(kTileDim * kBlockRows, 2)
__global__ void transpose_tiled_kernel(const float* __restrict__ input,
                                       float* __restrict__ output,
                                       int width, int height) {
  __shared__ float tile[kTileDim][kTileDim + 1];  // +1 to avoid shared memory bank conflicts

  const int x = blockIdx.x * kTileDim + threadIdx.x;
  const int y = blockIdx.y * kTileDim + threadIdx.y;

  // Load tile from input to shared memory with coalesced accesses
  #pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    int yj = y + j;
    if (x < width && yj < height) {
      int idx = yj * width + x;
      float v;
      #if __CUDA_ARCH__ >= 350
        v = __ldg(&input[idx]);
      #else
        v = input[idx];
      #endif
      tile[threadIdx.y + j][threadIdx.x] = v;
    }
  }

  __syncthreads();

  // Transposed coordinates for writing
  const int x2 = blockIdx.y * kTileDim + threadIdx.x;  // becomes column index in output
  const int y2 = blockIdx.x * kTileDim + threadIdx.y;  // becomes row index in output

  // Store from shared memory to output with coalesced accesses
  #pragma unroll
  for (int j = 0; j < kTileDim; j += kBlockRows) {
    int y2j = y2 + j;
    if (x2 < height && y2j < width) {
      float v = tile[threadIdx.x][threadIdx.y + j];
      // output is of size (new_height=width) x (new_width=height), row-major
      output[y2j * height + x2] = v;
    }
  }
}

}  // namespace

void transpose_f32(const float* input, float* output, int width, int height, int iters) {
  if (width <= 0 || height <= 0 || iters <= 0) return;

  const dim3 block(kTileDim, kBlockRows);
  const dim3 grid((width + kTileDim - 1) / kTileDim,
                  (height + kTileDim - 1) / kTileDim);

  for (int iter = 0; iter < iters; ++iter) {
    transpose_tiled_kernel<<<grid, block>>>(input, output, width, height);
  }
}
