#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_ptr.h>
#include <thrust/sort.h>
#include <thrust/execution_policy.h>

void sort_vec_i32(const int* input, int* output, int n, int iters)
{
    thrust::device_ptr<const int> d_in  = thrust::device_pointer_cast(input);
    thrust::device_ptr<int>       d_out = thrust::device_pointer_cast(output);

    for (int iter = 0; iter < iters; ++iter)
    {
        // copy input → output
        thrust::copy(thrust::device, d_in, d_in + n, d_out);
        // sort in-place on the output vector
        thrust::sort(thrust::device, d_out, d_out + n);
    }
}