#include "interface.h"
#include <thrust/reduce.h>
#include <thrust/execution_policy.h>
#include <thrust/functional.h>
#include <thrust/device_ptr.h>

float reduce_f32(const float* input, int n, int iters) {
    if (n <= 0) return 0.0f;
    auto dptr = thrust::device_pointer_cast(input);
    float sum = 0.0f;
    for (int i = 0; i < iters; ++i) {
        sum = thrust::reduce(thrust::device, dptr, dptr + n, 0.0f, thrust::plus<float>());
    }
    return sum;
}