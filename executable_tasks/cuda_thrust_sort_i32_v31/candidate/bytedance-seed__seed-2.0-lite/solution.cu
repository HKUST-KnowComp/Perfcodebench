#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_ptr.h>
#include <thrust/sort.h>
#include <thrust/copy.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    const auto d_input = thrust::device_pointer_cast(input);
    auto d_output = thrust::device_pointer_cast(output);
    
    for (int iter = 0; iter < iters; ++iter) {
        thrust::copy(d_input, d_input + n, d_output);
        thrust::sort(d_output, d_output + n);
    }
}