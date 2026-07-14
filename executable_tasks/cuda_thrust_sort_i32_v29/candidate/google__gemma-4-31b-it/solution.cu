#include "interface.h"
#include <thrust/device_ptr.h>
#include <thrust/sort.h>
#include <cuda_runtime.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    // Create thrust device pointers for the input and output arrays
    thrust::device_ptr<int> d_input = thrust::device_ptr<int>(const_cast<int*>(input));
    thrust::device_ptr<int> d_output = thrust::device_ptr<int>(output);

    for (int iter = 0; iter < iters; ++iter) {
        // Copy input to output first as thrust::sort sorts in-place
        thrust::copy(d_input, d_input + n, d_output);
        
        // Perform parallel sort on the output array
        thrust::sort(d_output, d_output + n);
    }
}