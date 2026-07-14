#include "interface.h"

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  uint64_t result_hash = 0;
  const size_t n = row_ids.size();
  const uint32_t* data = row_ids.data();
  const uint32_t* end = data + n;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;
    h = mix(h, 0); // offsets[0] is always 0

    const uint32_t* ptr = data;
    for (uint32_t r = 0; r < rows; ++r) {
      while (ptr < end && *ptr == r) {
        ++ptr;
      }
      h = mix(h, static_cast<uint64_t>(ptr - data));
    }
    result_hash = h;
  }
  return result_hash;
}
