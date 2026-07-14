#include "interface.h"
#include <thrust/sort.h>
#include <thrust/device_ptr.h>
#include <thrust/copy.h>
#include <cuda_runtime.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0 || iters <= 0) return;
    auto d_in  = thrust::device_pointer_cast(input);
    auto d_out = thrust::device_pointer_cast(output);
    for (int iter = 0; iter < iters; ++iter) {
        thrust::copy(d_in, d_in + n, d_out);
        thrust::sort(d_out, d_out + n);
    }
}
