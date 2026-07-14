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
                              const std::vector<int>&     lengths,
                              int batch, int max_len, int iters) {
  // upper bound on total tokens
  const std::size_t max_packed = static_cast<std::size_t>(batch) * max_len;
  std::vector<uint32_t> packed_tokens(max_packed);
  std::vector<uint32_t> positions(max_packed);

  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::size_t write = 0;
    const uint32_t* src = padded_tokens.data();

    for (int b = 0; b < batch; ++b) {
      const int len = lengths[b];
      const uint32_t* row = src + b * max_len;

      for (int i = 0; i < len; ++i) {
        packed_tokens[write] = row[i];
        positions[write]       = static_cast<uint32_t>(i);
        ++write;
      }
    }

    hash = 1469598103934665603ULL;
    const std::size_t n = write;

    // simple 4-way unroll; works for any n >= 0
    std::size_t i = 0;
    for (; i + 3 < n; i += 4) {
      hash = mix(hash, packed_tokens[i]);
      hash = mix(hash, positions[i]);
      hash = mix(hash, packed_tokens[i+1]);
      hash = mix(hash, positions[i+1]);
      hash = mix(hash, packed_tokens[i+2]);
      hash = mix(hash, positions[i+2]);
      hash = mix(hash, packed_tokens[i+3]);
      hash = mix(hash, positions[i+3]);
    }
    for (; i < n; ++i) {
      hash = mix(hash, packed_tokens[i]);
      hash = mix(hash, positions[i]);
    }
  }
  return hash;
}