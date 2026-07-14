#include "interface.h"
#include <thrust/device_ptr.h>
#include <thrust/sort.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    thrust::device_ptr<int> d_input(const_cast<int*>(input));
    thrust::device_ptr<int> d_output(output);

    for (int iter = 0; iter < iters; ++iter) {
        // Copy input to output first to preserve input integrity if needed,
        // though thrust::sort works in-place. To match the logic of moving 
        // data from input to output and sorting, we copy then sort.
        thrust::copy(d_input, d_input + n, d_output);
        thrust::sort(d_output, d_output + n);
    }
}