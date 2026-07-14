#include "interface.h"
#include <thrust/device_ptr.h>
#include <thrust/sort.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    thrust::device_ptr<int> d_input(const_cast<int*>(input));
    thrust::device_ptr<int> d_output(output);

    for (int iter = 0; iter < iters; ++iter) {
        // Copy input to output first to ensure we don't modify the original input if it's shared
        // and to satisfy the requirement of sorting the vector into the output buffer.
        // Note: If the task implies input and output are different buffers, we must copy.
        // Using thrust::copy is efficient.
        thrust::copy(d_input, d_input + n, d_output);
        thrust::sort(d_output, d_output + n);
    }
}