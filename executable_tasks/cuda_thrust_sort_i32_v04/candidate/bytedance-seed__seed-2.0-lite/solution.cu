#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/sort.h>
#include <thrust/copy.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    for (int iter = 0; iter < iters; ++iter) {
        // Match original kernel's behavior of copying input to output first
        thrust::copy(input, input + n, output);
        // Use Thrust's optimized GPU-accelerated ascending sort
        thrust::sort(output, output + n);
    }
}