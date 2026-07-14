#include "interface.h"
#include <cuda_runtime.h>
#include <cub/cub.cuh>

void sort_vec_i32(const int* input, int* output, int n, int iters) {
  if (n <= 0 || iters <= 0) return;

  void* d_temp_storage = nullptr;
  size_t temp_storage_bytes = 0;

  // Query temporary storage size for radix sort of 32-bit integers
  cub::DeviceRadixSort::SortKeys(
      d_temp_storage, temp_storage_bytes,
      const_cast<int*>(input), output,
      static_cast<int>(n), 0, 8 * static_cast<int>(sizeof(int)));

  if (temp_storage_bytes > 0) {
    cudaMalloc(&d_temp_storage, temp_storage_bytes);
  }

  for (int iter = 0; iter < iters; ++iter) {
    // Sort full 32-bit key space (handles signed ints correctly via CUB traits)
    cub::DeviceRadixSort::SortKeys(
        d_temp_storage, temp_storage_bytes,
        const_cast<int*>(input), output,
        static_cast<int>(n), 0, 8 * static_cast<int>(sizeof(int)));
  }

  if (d_temp_storage) cudaFree(d_temp_storage);
}
