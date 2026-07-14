#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_ptr.h>
#include <thrust://reduce.h>
#include <thrust/execution_policy.h>

float reduce_f32(const float* input, int n, int iters) {
    if (n <= 0) return 0.0f;

    thrust::device_ptr<const float> d_ptr(input);
    float d_result;

    // Perform the reduction 'iters' times as required by the interface
    for (int i = 0; i < iters; ++i) {
        d_result = thrust::reduce(d_ptr, d_ptr + n);
    }

    return d_result;
}