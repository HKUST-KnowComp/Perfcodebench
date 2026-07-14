#include "interface.h"
#include <cuda_runtime.h>
#include <algorithm>
#include <cstdlib>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
  if (n <= 0 || iters <= 0) return;

  size_t bytes = static_cast<size_t>(n) * sizeof(int);

  // Fast path for trivial sizes: just copy device-to-device per iteration.
  if (n <= 1) {
    for (int iter = 0; iter < iters; ++iter) {
      if (n == 1) {
        cudaMemcpy(output, input, bytes, cudaMemcpyDeviceToDevice);
      }
      // n == 0: nothing to do
    }
    return;
  }

  // Allocate pinned host memory for faster D2H/H2D copies; fallback to malloc if it fails.
  int* hbuf = nullptr;
  cudaError_t err = cudaMallocHost(reinterpret_cast<void**>(&hbuf), bytes);
  bool pinned = (err == cudaSuccess && hbuf != nullptr);
  if (!pinned) {
    hbuf = static_cast<int*>(std::malloc(bytes));
    if (!hbuf) {
      // Allocation failed; nothing we can do safely.
      return;
    }
  }

  for (int iter = 0; iter < iters; ++iter) {
    // Copy input from device to host
    cudaMemcpy(hbuf, input, bytes, cudaMemcpyDeviceToHost);

    // Sort on CPU (ascending)
    std::sort(hbuf, hbuf + n);

    // Copy sorted data back to device output
    cudaMemcpy(output, hbuf, bytes, cudaMemcpyHostToDevice);
  }

  if (pinned) {
    cudaFreeHost(hbuf);
  } else {
    std::free(hbuf);
  }
}
