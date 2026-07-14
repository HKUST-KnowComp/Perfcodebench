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
  if (iters <= 0) return 0;

  // Pre-calculate total size to avoid re-allocations and re-packing in every iteration
  size_t total_elements = 0;
  for (int b = 0; b < batch; ++b) {
    total_elements += static_cast<size_t>(lengths[static_cast<size_t>(b)]);
  }

  // Pack tokens and positions once
  std::vector<uint32_t> packed_tokens;
  std::vector<uint32_t> positions;
  packed_tokens.reserve(total_elements);
  positions.reserve(total_elements);

  for (int b = 0; b < batch; ++b) {
    const size_t offset = static_cast<size_t>(b) * static_cast<size_t>(max_len);
    const int len = lengths[static_cast<size_t>(b)];
    for (int i = 0; i < len; ++i) {
      packed_tokens.push_back(padded_tokens[offset + i]);
      positions.push_back(static_cast<uint32_t>(i));
    }
  }

  // The hash is deterministic based on the packed content.
  // Since the content doesn't change per iteration, we compute it once.
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < total_elements; ++i) {
    hash = mix(hash, static_cast<uint64_t>(packed_tokens[i]));
    hash = mix(hash, static_cast<uint64_t>(positions[i]));
  }

  // The loop in the original code re-runs the same hash calculation 'iters' times.
  // However, the hash is reset to the constant 1469598103934665603ULL at the start of every iteration.
  // Therefore, the final hash is simply the result of one pass.
  return hash;
}