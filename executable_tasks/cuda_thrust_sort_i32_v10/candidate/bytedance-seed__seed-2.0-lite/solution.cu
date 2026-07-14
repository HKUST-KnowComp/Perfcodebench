#include "interface.h"
#include <thrust/device_vector.h>
#include <thrust/sort.h>
#include <cuda_runtime.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;
    if (n == 1) {
        *output = *input;
        return;
    }
    // Allocate and initialize device memory once, outside the iteration loop
    thrust::device_vector<int> d_input(input, input + n);
    thrust::device_vector<int> d_output(n);
    
    for (int iter = 0; iter < iters; ++iter) {
        // Reset output to unsorted input each iteration to match baseline behavior
        d_output = d_input;
        // Launch Thrust's optimized GPU sort for int32
        thrust::sort(d_output.begin(), d_output.end());
    }
    // Copy final sorted result back to host output
    thrust::copy(d_output.begin(), d_output.end(), output);
}