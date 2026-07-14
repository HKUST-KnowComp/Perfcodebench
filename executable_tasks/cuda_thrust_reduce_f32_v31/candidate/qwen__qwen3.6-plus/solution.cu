#include "interface.h"
#include <thrust/reduce.h>
#include <thrust/device_ptr.h>

float reduce_f32(const float* input, int n, int iters) {
    float actual = 0.0f;
    if (n <= 0) return actual;
    thrust::device_ptr<const float> d_input(input);
    for (int i = 0; i < iters; ++i) {
        actual = thrust::reduce(d_input, d_input + n, 0.0f);
    }
    return actual;
}