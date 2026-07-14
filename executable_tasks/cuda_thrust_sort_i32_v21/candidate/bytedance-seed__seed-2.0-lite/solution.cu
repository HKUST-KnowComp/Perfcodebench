#include "interface.h"
#include <thrust/copy.h>
#include <thrust/sort.h>
#include <cuda_runtime.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    for (int iter = 0; iter < iters; ++iter) {
        // Reset output to original input before each sort, matching baseline behavior
        thrust::copy(input, input + n, output);
        // Parallel ascending sort matching baseline's insertion sort order
        thrust::sort(output, output + n);
    }
}