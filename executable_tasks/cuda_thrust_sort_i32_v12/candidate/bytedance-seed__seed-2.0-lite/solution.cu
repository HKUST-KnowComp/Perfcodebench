#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/copy.h>
#include <thrust/sort.h>
#include <thrust/device/execution_policy.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    for (int iter = 0; iter < iters; ++iter) {
        // Match baseline's initial input-to-output copy step
        thrust::copy_n(thrust::device, input, n, output);
        // Parallel ascending sort matching the baseline's insertion sort order
        thrust::sort(thrust::device, output, output + n);
    }
}