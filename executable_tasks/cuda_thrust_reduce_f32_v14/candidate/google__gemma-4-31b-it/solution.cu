#include "interface.h"
#include <thrust/reduce.h>
#include <thrust/device_ptr.h>
#include <thrust/execution_policy.h>

float reduce_f32(const float* input, int n, int iters) {
    if (n <= 0) return 0.0f;

    // Wrap the raw pointer into a thrust device pointer
    thrust::device_ptr<const float> dev_ptr(input);
    float total_sum = 0.0f;

    // The task requires running the reduction 'iters' times
    // We perform the reduction in each iteration
    for (int iter = 0; iter < iters; ++iter) {
        total_sum = thrust::reduce(thrust::device, dev_ptr, dev_ptr + n, 0.0f, thrust::plus<float>());
    }

    return total_sum;
}