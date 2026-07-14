#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_ptr.h>
#include <thrust/sort.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    for (int iter = 0; iter < iters; ++iter) {
        if (n <= 0) continue;
        const auto in_ptr = thrust::device_pointer_cast(input);
        auto out_ptr = thrust::device_pointer_cast(output);
        thrust::sort_copy(in_ptr, in_ptr + n, out_ptr);
    }
}