#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
  if (n <= 0 || iters <= 0) return;

  void* d_temp_storage = nullptr;
  size_t temp_bytes = 0;

  // Query temp storage size for 32-bit integer radix sort (full bit range)
  cub::DeviceRadixSort::SortKeys(
      d_temp_storage, temp_bytes,
      input, output,
      static_cast<int>(n),
      0, 8 * sizeof(int));

  if (temp_bytes == 0) return;

  cudaMalloc(&d_temp_storage, temp_bytes);

  // Perform the sort repeatedly as requested
  for (int iter = 0; iter < iters; ++iter) {
    cub::DeviceRadixSort::SortKeys(
        d_temp_storage, temp_bytes,
        input, output,
        static_cast<int>(n),
        0, 8 * sizeof(int));
  }

  cudaFree(d_temp_storage);
}
