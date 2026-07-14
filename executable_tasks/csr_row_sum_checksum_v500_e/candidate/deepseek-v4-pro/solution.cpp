#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int /*iters*/) {
  uint64_t hash = 1469598103934665603ULL;
  if (row_ptr.size() <= 1) {
    return hash;
  }
  const std::size_t nrows = row_ptr.size() - 1;
  const uint32_t* vals = values.data();
  for (std::size_t r = 0; r < nrows; ++r) {
    const uint32_t* start = vals + row_ptr[r];
    const uint32_t* end = vals + row_ptr[r + 1];
    uint64_t sum = 0;
    for (const uint32_t* p = start; p < end; ++p) {
      sum += static_cast<uint64_t>(*p);
    }
    hash ^= sum;
    hash *= 1099511628211ULL;
  }
  return hash;
}