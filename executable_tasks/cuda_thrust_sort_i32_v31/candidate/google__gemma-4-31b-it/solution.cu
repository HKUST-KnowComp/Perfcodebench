#include "interface.h"
#include <thrust/device_ptr.h>
#include <thrust/sort.h>
#include <cuda_runtime.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    // Create device pointers for Thrust
    thrust::device_ptr<const int> d_input(input);
    thrust::device_ptr<int> d_output(output);

    // Copy input to output first because thrust::sort sorts in-place
    thrust::copy(d_input, d_input + n, d_output);

    for (int iter = 0; iter < iters; ++iter) {
        // thrust::sort for integers typically uses a highly optimized radix sort
        thrust::sort(d_output, d_output + n);
    }
}