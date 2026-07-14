#include "interface.h"
#include <cuda_runtime.h>
namespace {
__global__ void atomic_reduce_kernel(const int* input, int* output, int n) {
  for (int idx = blockIdx.x * blockDim.x + threadIdx.x; idx < n; idx += blockDim.x * gridDim.x) atomicAdd(output, input[idx]);
}
}  // namespace
void add_reduce_i32(const int* input, int* output, int n, int iters) {
  for (int iter = 0; iter < iters; ++iter) { cudaMemset(output, 0, sizeof(int)); atomic_reduce_kernel<<<1024, 256>>>(input, output, n); }
}
