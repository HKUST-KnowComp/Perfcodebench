#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/reduce.h>
#include <thrust/device_ptr.h>

float reduce_f32(const float* input, int n, int iters) {
    float result = 0.0f;
    thrust::device_ptr<const float> dev_ptr(input);
    for (int i = 0; i < iters; ++i) {
        result = thrust::reduce(dev_ptr, dev_ptr + n, 0.0f);
    }
    return result;
}