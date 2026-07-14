#include "interface.h"
#include <thrust/device_ptr.h>
#include <thrust/reduce.h>
#include <cuda_runtime.h>

float reduce_f32(const float* input, int n, int iters) {
    if (n <= 0) return 0.0f;
    
    thrust::device_ptr<const float> dev_ptr(input);
    float result = 0.0f;
    
    // Warm-up and execution loop
    // Thrust's reduce is highly optimized for GPU architectures
    for (int iter = 0; iter < iters; ++iter) {
        result = thrust::reduce(dev_ptr, dev_ptr + n, 0.0f, thrust::plus<float>());
    }
    
    return result;
}