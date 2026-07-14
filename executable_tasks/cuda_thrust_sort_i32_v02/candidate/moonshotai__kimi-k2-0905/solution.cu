#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_vector.h>
#include <thrust/sort.h>
#include <thrust/copy.h>

void sort_vec_i32(const int* input, int* output, int n, int iters)
{
    if (n <= 0 || iters <= 0) return;

    thrust::device_vector<int> d_vec(n);

    for (int iter = 0; iter < iters; ++iter)
    {
        // Copy input to temporary vector
        thrust::copy(thrust::device, input, input + n, d_vec.begin());

        // Sort the vector in-place
        thrust::sort(thrust::device, d_vec.begin(), d_vec.end());

        // Copy sorted data back to output
        thrust::copy(thrust::device, d_vec.begin(), d_vec.end(), output);
    }
}
