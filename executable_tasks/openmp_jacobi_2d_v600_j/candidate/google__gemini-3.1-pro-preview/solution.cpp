#include "interface.h"
#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

/**
 * FNV-1a hash for checksum calculation.
 * This must remain bit-identical to the reference implementation.
 */
uint64_t checksum_u32(const std::vector<uint32_t>& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t v : data) {
    hash ^= static_cast<uint64_t>(v);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t jacobi_checksum(const std::vector<uint32_t>& input, int rows, int cols, int steps) {
  if (rows <= 0 || cols <= 0) return 0;
  if (steps <= 0) return checksum_u32(input);

  // Use two buffers to avoid in-place modification issues.
  // Initialize both with input to ensure boundaries (which never change) are set.
  std::vector<uint32_t> a = input;
  std::vector<uint32_t> b = input;

  uint32_t* p_a = a.data();
  uint32_t* p_b = b.data();

  // Only process the interior of the grid.
  // Boundaries (row 0, row rows-1, col 0, col cols-1) remain constant.
  for (int step = 0; step < steps; ++step) {
    #pragma omp parallel for schedule(static)
    for (int row = 1; row < rows - 1; ++row) {
      const size_t row_offset = (size_t)row * cols;
      const uint32_t* __restrict__ src = p_a + row_offset;
      uint32_t* __restrict__ dst = p_b + row_offset;

      for (int col = 1; col < cols - 1; ++col) {
        // Jacobi-like smoothing: (4*center + left + right + up + down) / 8
        dst[col] = (src[col] * 4u +
                    src[col - 1] +
                    src[col + 1] +
                    src[col - cols] +
                    src[col + cols]) / 8u;
      }
    }
    // Swap pointers for the next iteration
    std::swap(p_a, p_b);
  }

  // The final result is in p_a because of the swap at the end of the loop.
  // If steps was even, p_a points to a.data(). If odd, p_a points to b.data().
  // We need to ensure the checksum is calculated on the correct final state.
  if (steps % 2 != 0) {
    return checksum_u32(b);
  } else {
    return checksum_u32(a);
  }
}
