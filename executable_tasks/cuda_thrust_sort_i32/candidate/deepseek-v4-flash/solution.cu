#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_ptr.h>
#include <thrust/sort.h>
#include <thrust/copy.h>
#include <thrust/execution_policy.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    for (int i = 0; i < iters; ++i) {
        thrust::copy(thrust::device, input, input + n, output);
        thrust::sort(thrust::device, output, output + n);
    }
}