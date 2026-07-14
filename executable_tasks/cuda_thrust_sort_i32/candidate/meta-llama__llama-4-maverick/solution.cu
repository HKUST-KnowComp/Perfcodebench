#include "interface.h"
#include <thrust/sort.h>
#include <thrust/device_vector.h>
#include <thrust/device_ptr.h>
#include <cuda_runtime.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
  thrust::device_vector<int> d_input(input, input + n);
  thrust::device_vector<int> d_output(n);
  for (int iter = 0; iter < iters; ++iter) {
    thrust::copy(d_input.begin(), d_input.end(), d_output.begin());
    thrust::sort(d_output.begin(), d_output.end());
    cudaMemcpy(output, thrust::raw_pointer_cast(d_output.data()), n * sizeof(int), cudaMemcpyDeviceToDevice);
  }
}