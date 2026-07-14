#include "interface.h"

#include <cuda_runtime.h>

namespace {

__global__ void sequential_scan_kernel(const int* input, int* output, int n) {
  if (blockIdx.x != 0 || threadIdx.x != 0) {
    return;
  }
  int running = 0;
  for (int i = 0; i < n; ++i) {
    output[i] = running;
    running += input[i];
  }
}

}  // namespace

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
  for (int iter = 0; iter < iters; ++iter) {
    sequential_scan_kernel<<<1, 1>>>(input, output, n);
  }
}
