#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_ptr.h>
#include <thrust/copy.h>
#include <thrust/sort.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    for (int iter = 0; iter < iters; ++iter) {
        thrust::copy(thrust::device_pointer_cast(input),
                     thrust::device_pointer_cast(input) + n,
                     thrust::device_pointer_cast(output));
        thrust::sort(thrust::device_pointer_cast(output),
                     thrust::device_pointer_cast(output) + n);
    }
}