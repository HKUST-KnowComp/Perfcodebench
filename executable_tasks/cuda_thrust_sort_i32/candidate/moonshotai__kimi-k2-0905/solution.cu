#include "interface.h"
#include <thrust/device_ptr.h>
#include <thrust/sort.h>
#include <thrust/device_vector.h>

void sort_vec_i32(const int* input, int* output, int n, int iters)
{
    thrust::device_vector<int> temp(n);
    for (int iter = 0; iter < iters; ++iter)
    {
        thrust::copy(input, input + n, temp.begin());
        thrust::sort(temp.begin(), temp.end());
        thrust::copy(temp.begin(), temp.end(), output);
    }
}