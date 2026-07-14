#include "interface.h"
#include <thrust/device_ptr.h>
#include <thrust://sort.h>
#include <thrust/execution_policy.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    thrust::device_ptr<int> d_input(const_cast<int*>(input));
    thrust::device_ptr<int> d_output(output);

    for (int iter = 0; iter < iters; ++iter) {
        // Copy input to output first to preserve input integrity if needed, 
        // though thrust::sort usually works on the range provided.
        // To match the behavior of a sort that produces a sorted output from an input:
        thrust::copy(thrust::device, d_input, d_input + n, d_output);
        thrust::sort(thrust::device, d_output, d_output + n);
    }
}