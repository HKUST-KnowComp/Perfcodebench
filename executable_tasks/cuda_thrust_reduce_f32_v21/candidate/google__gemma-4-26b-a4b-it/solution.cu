#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_ptr.h>
#include <thrust://reduce.h>
#include <thrust/execution_policy.h>

float reduce_f32(const float* input, int n, int iters) {
    if (n <= 0) return 0.0f;

    thrust::device_ptr<const float> d_ptr(input);
    float actual = 0.0f;

    // Perform the reduction 'iters' times as required by the interface
    // Note: In a real scenario, we'd want to avoid redundant launches, 
    // but we must follow the 'iters' loop logic provided in the baseline.
    for (int iter = 0; iter < iters; ++iter) {
        actual = thrust::reduce(d_ptr, d_ptr + n, thrust::device);
    }

    return actual;
}