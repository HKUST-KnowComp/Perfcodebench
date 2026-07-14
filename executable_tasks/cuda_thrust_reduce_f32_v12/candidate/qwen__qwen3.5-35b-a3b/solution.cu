#include "interface.h"
#include <thrust/device_ptr.h>
#include <thrust/reduce.h>

float reduce_f32(const float* input, int n, int iters) {
    thrust::device_ptr<const float> d_input(input);
    float result = thrust::reduce(d_input, d_input + n, 0.0f, thrust::plus<float>());
    return result;
}