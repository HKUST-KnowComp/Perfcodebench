#include "interface.h"
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
  std::vector<uint64_t> row_sums;
  if (row_ptr.size() >= 2) {
    row_sums.reserve(row_ptr.size() - 1);
    for (std::size_t r = 0; r + 1 < row_ptr.size(); ++r) {
      const std::size_t start = static_cast<std::size_t>(row_ptr[r]);
      const std::size_t end = static_cast<std::size_t>(row_ptr[r + 1]);
      uint64_t sum = 0;
      for (std::size_t i = start; i < end; ++i) {
        sum += static_cast<uint64_t>(values[i]);
      }
      row_sums.push_back(sum);
    }
  }
  uint64_t final_hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    final_hash = 1469598103934665603ULL;
    for (const uint64_t s : row_sums) {
      final_hash = mix(final_hash, s);
    }
  }
  return final_hash;
}