#include "interface.h"
#include <thrust/sort.h>
#include <thrust/copy.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    for (int iter = 0; iter < iters; ++iter) {
        thrust::copy(input, input + n, output);
        thrust::sort(output, output + n);
    }
}