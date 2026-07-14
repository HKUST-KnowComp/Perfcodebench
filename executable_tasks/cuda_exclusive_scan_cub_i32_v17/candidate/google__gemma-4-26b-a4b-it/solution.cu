#include "interface.h"
#include <thrust/scan.h>
#include <thrust/device_ptr.h>

void exclusive_scan_i32(const int* input, int* output, int n, int iters) {
    if (n <= 0) return;

    thrust::device_ptr<const int> d_input(input);
    thrust::device_ptr<int> d_output(output);

    for (int iter = 0; iter < iters; ++iter) {
        thrust::exclusive_scan(d_input, d_input + n, d_output);
    }
}