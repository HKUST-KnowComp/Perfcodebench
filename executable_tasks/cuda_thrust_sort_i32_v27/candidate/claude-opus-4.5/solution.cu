#include "interface.h"
#include <thrust/device_ptr.h>
#include <thrust/sort.h>
#include <thrust/copy.h>
#include <cuda_runtime.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    thrust::device_ptr<const int> d_input(input);
    thrust::device_ptr<int> d_output(output);
    for (int iter = 0; iter < iters; ++iter) {
        thrust::copy(d_input, d_input + n, d_output);
        thrust::sort(d_output, d_output + n);
    }
}
