#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
  if (iters <= 0) return 0;

  // Pre-calculate total size to avoid re-allocations and re-packing inside the loop
  size_t total_elements = 0;
  for (int b = 0; b < batch; ++b) {
    total_elements += static_cast<size_t>(lengths[static_cast<size_t>(b)]);
  }

  if (total_elements == 0) {
    // If no elements, the hash is initialized but the loop doesn't run.
    // Based on baseline, hash is reset to FNV offset inside the loop.
    return 1469598103934665603ULL;
  }

  std::vector<uint32_t> packed_tokens;
  std::vector<uint32_t> positions;
  packed_tokens.reserve(total_elements);
  positions.reserve(total_elements);

  // Pack once outside the loop
  for (int b = 0; b < batch; ++b) {
    const size_t offset = static_cast<size_t>(b) * static_cast<size_t>(max_len);
    const int len = lengths[static_cast<size_t>(b)];
    for (int i = 0; i < len; ++i) {
      packed_tokens.push_back(padded_tokens[offset + i]);
      positions.push_back(static_cast<uint32_t>(i));
    }
  }

  uint64_t hash = 0;
  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint32_t* p_tokens = packed_tokens.data();
  const uint32_t* p_positions = positions.data();

  // The baseline resets hash to FNV_OFFSET at the start of every iteration
  // and the final value returned is the result of the last iteration.
  for (int iter = 0; iter < iters; ++iter) {
    hash = FNV_OFFSET;
    for (size_t i = 0; i < total_elements; ++i) {
      hash = mix(hash, static_cast<uint64_t>(p_tokens[i]));
      hash = mix(hash, static_cast<uint64_t>(p_positions[i]));
    }
  }

  return hash;
}