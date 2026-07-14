#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
  // Precompute total packed tokens and positions once
  std::size_t total = 0;
  for (int b = 0; b < batch; ++b) {
    total += static_cast<std::size_t>(lengths[b]);
  }
  std::vector<uint32_t> packed(total);
  std::vector<uint32_t> positions(total);
  std::size_t idx = 0;
  for (int b = 0; b < batch; ++b) {
    int len = lengths[b];
    std::size_t base = static_cast<std::size_t>(b) * static_cast<std::size_t>(max_len);
    for (int i = 0; i < len; ++i) {
      packed[idx] = padded_tokens[base + static_cast<std::size_t>(i)];
      positions[idx] = static_cast<uint32_t>(i);
      ++idx;
    }
  }

  // Use raw pointers for faster access
  const uint32_t* packed_ptr = packed.data();
  const uint32_t* positions_ptr = positions.data();
  std::size_t n = total;

  // Main loop over iterations
  uint64_t hash = 1469598103934665603ULL;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < n; ++i) {
      hash ^= packed_ptr[i];
      hash *= 1099511628211ULL;
      hash ^= positions_ptr[i];
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
