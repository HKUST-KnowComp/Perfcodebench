#include "interface.h"
#include <cuda_runtime.h>
namespace {
__global__ void insertion_sort_kernel(const int* input, int* output, int n) {
  if (blockIdx.x != 0 || threadIdx.x != 0) return;
  for (int i = 0; i < n; ++i) output[i] = input[i];
  for (int i = 1; i < n; ++i) { int key = output[i]; int j = i - 1; while (j >= 0 && output[j] > key) { output[j + 1] = output[j]; --j; } output[j + 1] = key; }
}
}  // namespace
void sort_vec_i32(const int* input, int* output, int n, int iters) { for (int iter = 0; iter < iters; ++iter) insertion_sort_kernel<<<1, 1>>>(input, output, n); }
