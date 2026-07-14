#include "interface.h"
#include <thrust/reduce.h>
#include <thrust/device_ptr.h>
#include <thrust/execution_policy.h>

float reduce_f32(const float* input, int n, int iters) {
    if (n == 0) return 0.0f;
    thrust::device_ptr<const float> d_input(input);
    float sum = 0.0f;
    for (int i = 0; i < iters; ++i) {
        sum = thrust::reduce(thrust::device, d_input, d_input + n);
    }
    return sum;
}
