#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_vector.h>
#include <thrust/sort.h>
#include <thrust/copy.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    thrust::device_vector<int> d_input(n);
    for (int iter = 0; iter < iters; ++iter) {
        thrust::copy(input, input + n, d_input.begin());
        thrust::sort(d_input.begin(), d_input.end());
        thrust::copy(d_input.begin(), d_input.end(), output);
    }
}