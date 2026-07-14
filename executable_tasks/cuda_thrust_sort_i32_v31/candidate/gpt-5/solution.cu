#include "interface.h"
#include <cuda_runtime.h>
#include <thrust/device_ptr.h>
#include <thrust/sort.h>
#include <thrust/system/cuda/execution_policy.h>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0 || iters <= 0) return;

    cudaStream_t stream;
    cudaStreamCreateWithFlags(&stream, cudaStreamNonBlocking);

    thrust::device_ptr<int> out_begin(output);
    thrust::device_ptr<int> out_end = out_begin + static_cast<size_t>(n);

    for (int iter = 0; iter < iters; ++iter) {
        if (input != output) {
            cudaMemcpyAsync(output, input, static_cast<size_t>(n) * sizeof(int), cudaMemcpyDeviceToDevice, stream);
        }
        if (n > 1) {
            thrust::stable_sort(thrust::cuda::par.on(stream), out_begin, out_end);
        }
    }

    cudaStreamSynchronize(stream);
    cudaStreamDestroy(stream);
}
