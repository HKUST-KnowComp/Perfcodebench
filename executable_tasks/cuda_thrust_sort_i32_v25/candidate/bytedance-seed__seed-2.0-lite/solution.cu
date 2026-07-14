#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/sort.h>
#include <thrust/device_vector.h>
#include <thrust/copy.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
  // Edge case handling for empty or single-element inputs
  if (n <= 1) {
    for (int iter = 0; iter < iters; ++iter) {
      cudaMemcpy(output, input, n * sizeof(int), cudaMemcpyHostToDevice);
    }
    return;
  }

  // Allocate persistent device memory to avoid repeated allocation overhead
  thrust::device_vector<int> d_input(input, input + n);
  thrust::device_vector<int> d_output(n);

  // Match baseline loop structure to run sort for all required timing iterations
  for (int iter = 0; iter < iters; ++iter) {
    // Reset output array to original input each iteration to sort the full dataset every time
    thrust::copy(d_input.begin(), d_input.end(), d_output.begin());
    // Thrust's optimized parallel GPU sort, produces identical output to CPU std::sort
    thrust::sort(d_output.begin(), d_output.end());
  }

  // Copy final sorted result back to host output to meet correctness requirements
  thrust::copy(d_output.begin(), d_output.end(), output);
}