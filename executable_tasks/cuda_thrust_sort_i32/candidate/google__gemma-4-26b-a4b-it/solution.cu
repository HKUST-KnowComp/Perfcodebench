#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_ptr.h>
#include <thrust::sort.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    thrust::device_ptr<const int> d_input(input);
    thrust::device_ptr<int> d_output(output);

    for (int iter = 0; iter < iters; ++iter) {
        // Copy input to output first to ensure we don't modify the original input
        // and to prepare the output buffer for sorting.
        thrust::copy(d_input, d_input + n, d_output);
        // Perform the high-performance sort on the output buffer.
        thrust::sort(d_output, d_output + n);
    }
}