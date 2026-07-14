#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/reduce.h>
#include <thrust/device_ptr.h>
#include <thrust/functional.h>

float reduce_f32(const float* input, int n, int iters) {
    thrust::device_ptr<const float> d_in = thrust::device_pointer_cast<const float>(input);
    float actual = 0.0f;
    
    for (int iter = 0; iter < iters; ++iter) {
        actual = thrust::reduce(d_in, d_in + n, 0.0f, thrust::plus<float>());
    }
    
    return actual;
}