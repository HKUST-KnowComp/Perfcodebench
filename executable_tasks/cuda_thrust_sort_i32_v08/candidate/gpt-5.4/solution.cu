#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_ptr.h>
#include <thrust/copy.h>
#include <thrust/sort.h>

namespace {

inline int* get_scratch(int n) {
  static int* buf = nullptr;
  static int cap = 0;
  if (n > cap) {
    if (buf) cudaFree(buf);
    cudaMalloc(&buf, static_cast<size_t>(n) * sizeof(int));
    cap = n;
  }
  return buf;
}

}  // namespace

void sort_vec_i32(const int* input, int* output, int n, int iters) {
  if (n <= 0 || iters <= 0) return;

  int* scratch = get_scratch(n);
  thrust::device_ptr<const int> in_ptr(input);
  thrust::device_ptr<int> scratch_ptr(scratch);
  thrust::device_ptr<int> out_ptr(output);

  for (int iter = 0; iter < iters; ++iter) {
    thrust::copy(in_ptr, in_ptr + n, scratch_ptr);
    thrust::sort(scratch_ptr, scratch_ptr + n);
    thrust::copy(scratch_ptr, scratch_ptr + n, out_ptr);
  }
}
