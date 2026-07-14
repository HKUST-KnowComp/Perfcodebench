#include "interface.h"
#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens,
                              const std::vector<int>& lengths,
                              int batch, int max_len, int iters) {
  // Precompute packed tokens and positions once.
  std::size_t total = 0;
  for (int b = 0; b < batch; ++b) {
    total += static_cast<std::size_t>(lengths[b]);
  }
  std::vector<uint32_t> packed_tokens;
  std::vector<uint32_t> positions;
  packed_tokens.reserve(total);
  positions.reserve(total);
  for (int b = 0; b < batch; ++b) {
    std::size_t base = static_cast<std::size_t>(b) * static_cast<std::size_t>(max_len);
    int len = lengths[b];
    for (int i = 0; i < len; ++i) {
      packed_tokens.push_back(padded_tokens[base + static_cast<std::size_t>(i)]);
      positions.push_back(static_cast<uint32_t>(i));
    }
  }

  // Compute hash over packed data.
  uint64_t hash = 0;
  const uint32_t* pt_data = packed_tokens.data();
  const uint32_t* pos_data = positions.data();
  std::size_t n = packed_tokens.size();
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < n; ++i) {
      hash = mix(hash, pt_data[i]);
      hash = mix(hash, pos_data[i]);
    }
  }
  return hash;
}
