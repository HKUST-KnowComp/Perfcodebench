#include "interface.h"
#include <thrust/device_ptr.h>
#include <thrust/device_vector.h>
#include <thrust/sort.h>
#include <thrust/execution_policy.h>

void sort_vec_i32(const int* input, int* output, int n, int iters)
{
    if (n <= 0 || iters <= 0) return;

    // Ensure Thrust's caching allocator is in place (usually it is by default).
    // We explicitly allocate host vectors only once to avoid per-iteration overhead.
    thrust::device_vector<int> d_input(input, input + n);
    thrust::device_vector<int> d_output(n);

    int* d_in  = thrust::raw_pointer_cast(d_input.data());
    int* d_out = thrust::raw_pointer_cast(d_output.data());

    for (int iter = 0; iter < iters; ++iter)
    {
        // Copy input to temporary buffer
        thrust::copy(thrust::device, d_in, d_in + n, d_out);

        // Sort the buffer in-place on the device
        thrust::stable_sort(thrust::device, d_out, d_out + n);

        // If this is the last iteration, copy the sorted data to the final output pointer
        if (iter == iters - 1)
            thrust::copy(thrust::device, d_out, d_out + n, output);
    }
}