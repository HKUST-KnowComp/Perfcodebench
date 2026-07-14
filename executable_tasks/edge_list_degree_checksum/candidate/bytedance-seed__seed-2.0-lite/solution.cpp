#include "interface.h"
#include <vector>
#include <cstring>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t degree_checksum(
    const std::vector<uint32_t>& src,
    const std::vector<uint32_t>& dst,
    uint32_t vertices,
    int iters) {
  (void)dst;
  // Preallocate contiguous degree array once to avoid repeated allocations
  std::vector<uint32_t> degrees(static_cast<std::size_t>(vertices), 0);
  uint64_t hash = 0;
  const std::size_t src_size = src.size();
  const std::size_t degree_byte_count = degrees.size() * sizeof(uint32_t);

  for (int iter = 0; iter < iters; ++iter) {
    // Bulk reset degrees to zero with optimized libc memset
    std::memset(degrees.data(), 0, degree_byte_count);
    // Count out-degrees directly without storing edge data
    for (std::size_t i = 0; i < src_size; ++i) {
      degrees[static_cast<std::size_t>(src[i])]++;
    }
    // Exact same hash computation as baseline to preserve checksum correctness
    hash = 1469598103934665603ULL;
    for (uint32_t degree : degrees) {
      hash = mix(hash, degree);
    }
  }
  return hash;
}