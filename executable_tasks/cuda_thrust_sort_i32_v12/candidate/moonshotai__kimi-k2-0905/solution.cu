#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_vector.h>
#include <thrust/sort.h>
#include <thrust/execution_policy.h>

void sort_vec_i32(const int* input, int* output, int n, int iters)
{
    thrust::device_vector<int> d_in(input, input + n);
    thrust::device_vector<int> d_out(n);

    for (int iter = 0; iter < iters; ++iter)
    {
        // copy input to workspace (async)
        thrust::copy(thrust::device, d_in.begin(), d_in.end(), d_out.begin());
        // sort in-place on the workspace
        thrust::sort(thrust::device, d_out.begin(), d_out.end());
    }

    // copy final sorted data back to host output buffer
    thrust::copy(d_out.begin(), d_out.end(), output);
}