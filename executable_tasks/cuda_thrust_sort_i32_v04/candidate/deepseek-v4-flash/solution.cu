#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/sort.h>
#include <thrust/device_ptr.h>
#include <thrust/copy.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    if (n <= 1) {
        for (int iter = 0; iter < iters; ++iter) {
            if (n == 1) output[0] = input[0];
        }
        return;
    }
    thrust::device_ptr<const int> d_input(input);
    thrust::device_ptr<int> d_output(output);
    for (int iter = 0; iter < iters; ++iter) {
        thrust::copy(d_input, d_input + n, d_output);
        thrust::sort(d_output, d_output + n);
    }
}
