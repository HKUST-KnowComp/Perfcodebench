#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_ptr.h>
#include <thrust/reduce.h>
float reduce_f32(const float* input, int n, int iters) {
    float actual = 0.0f;
    for (int iter = 0; iter < iters; ++iter) {
        actual = thrust::reduce(thrust::device, input, input + n);
    }
    return actual;
}