#include "interface.h"
#include <thrust/reduce.h>
#include <thrust/device_ptr.h>
#include <cuda_runtime.h>

float reduce_f32(const float* input, int n, int iters) {
    if (n <= 0) return 0.0f;
    
    thrust::device_ptr<const float> d_ptr(input);
    float result = 0.0f;

    // Warm-up and execution loop
    for (int iter = 0; iter < iters; ++iter) {
        result = thrust::reduce(d_ptr, d_ptr + n, 0.0f, thrust::plus<float>());
    }
    
    return result;
}