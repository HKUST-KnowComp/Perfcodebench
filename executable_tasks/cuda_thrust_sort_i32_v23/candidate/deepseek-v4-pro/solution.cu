#include "interface.h"
#include <thrust/device_vector.h>
#include <thrust/sort.h>
#include <thrust/copy.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    if (n == 0) return;
    thrust::device_vector<int> d_vec(input, input + n);
    for (int i = 0; i < iters; ++i) {
        thrust::sort(d_vec.begin(), d_vec.end());
    }
    thrust::copy(d_vec.begin(), d_vec.end(), output);
}