#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_vector.h>
#include <thrust/copy.h>
#include <thrust/radix_sort.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    // Preallocate device memory once to eliminate redundant allocation overhead
    thrust::device_vector<int> d_input(input, input + n);
    thrust::device_vector<int> d_output(n);

    // Run sort for the required number of iterations to match baseline timing requirements
    for (int iter = 0; iter < iters; ++iter) {
        // Refresh with unsorted input each iteration to ensure every sort operates on raw input
        thrust::copy(d_input.begin(), d_input.end(), d_output.begin());
        // Radix sort is the fastest specialized sort for int32 values on CUDA
        thrust::radix_sort(d_output.begin(), d_output.end());
    }

    // Copy final sorted result back to host output
    thrust::copy(d_output.begin(), d_output.end(), output);
}