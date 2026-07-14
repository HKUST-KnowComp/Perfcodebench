#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/reduce.h>
#include <thrust/device_ptr.h>
float reduce_f32(const float* input, int n, int iters) { float* d_out = nullptr; cudaMalloc(&d_out, sizeof(float)); float actual = 0.0f; for (int iter = 0; iter < iters; ++iter) { actual = thrust::reduce(thrust::device_pointer_cast(input), thrust::device_pointer_cast(input + n), 0.0f, thrust::plus<float>()); } cudaMemcpy(d_out, &actual, sizeof(float), cudaMemcpyHostToDevice); cudaFree(d_out); return actual; }
