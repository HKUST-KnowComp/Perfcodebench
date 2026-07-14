#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/sort.h>
#include <thrust/copy.h>
#include <thrust/device_ptr.h>
#include <thrust/execution_policy.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    auto d_in = thrust::device_pointer_cast(input);
    auto d_out = thrust::device_pointer_cast(output);
    for (int i = 0; i < iters; ++i) {
        thrust::copy(thrust::device, d_in, d_in + n, d_out);
        thrust::sort(thrust::device, d_out, d_out + n);
    }
}