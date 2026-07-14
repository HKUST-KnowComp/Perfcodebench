#include "interface.h"
#include <thrust/device_ptr.h>
#include <thrust/sort.h>
#include <cuda_runtime.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    // Create device pointers for thrust
    thrust::device_ptr<int> d_input(const_cast<int*>(input));
    thrust::device_ptr<int> d_output(output);

    for (int iter = 0; iter < iters; ++iter) {
        // Copy input to output first to avoid modifying input if it's read-only
        thrust::copy(d_input, d_input + n, d_output);
        // Perform parallel sort on the output buffer
        thrust::sort(d_output, d_output + n);
    }
}