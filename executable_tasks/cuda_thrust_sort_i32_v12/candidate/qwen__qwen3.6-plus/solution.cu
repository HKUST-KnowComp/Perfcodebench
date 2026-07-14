#include "interface.h"
#include <thrust/sort.h>
#include <thrust/copy.h>
#include <thrust/device_ptr.h>
#include <cuda_runtime.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    thrust::device_ptr<const int> d_in(input);
    thrust::device_ptr<int> d_out(output);
    for (int i = 0; i < iters; ++i) {
        thrust::copy(d_in, d_in + n, d_out);
        thrust::sort(d_out, d_out + n);
    }
}
