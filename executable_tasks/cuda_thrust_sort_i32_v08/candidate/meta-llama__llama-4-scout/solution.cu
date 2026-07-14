#include "interface.h"
#include <thrust/sort.h>
#include <thrust/device_vector.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
  thrust::device_ptr<const int> input_ptr(input);
  thrust::device_ptr<int> output_ptr(output);
  for (int iter = 0; iter < iters; ++iter) {
    thrust::device_vector<int> vec(input_ptr, input_ptr + n);
    thrust::sort(vec.begin(), vec.end());
    thrust::copy(vec.begin(), vec.end(), output_ptr);
  }
}