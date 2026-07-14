#include "interface.h"
#include <thrust/reduce.h>
#include <thrust/device_ptr.h>

float reduce_f32(const float* input, int n, int iters) {
    float result = 0.0f;
    thrust::device_ptr<const float> d_input(input);
    for (int i = 0; i < iters; ++i) {
        result = thrust::reduce(d_input, d_input + n, 0.0f);
    }
    return result;
}