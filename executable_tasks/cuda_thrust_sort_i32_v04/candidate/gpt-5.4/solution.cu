#include "interface.h"

#include <cuda_runtime.h>
#include <thrust/device_ptr.h>
#include <thrust/copy.h>
#include <thrust/sort.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
  if (n <= 0 || iters <= 0) return;

  thrust::device_ptr<const int> in_ptr(input);
  thrust::device_ptr<int> out_ptr(output);

  for (int iter = 0; iter < iters; ++iter) {
    thrust::copy(in_ptr, in_ptr + n, out_ptr);
    thrust::sort(out_ptr, out_ptr + n);
  }
}
