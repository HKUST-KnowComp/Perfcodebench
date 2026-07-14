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

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens,
                              const std::vector<int>& lengths,
                              int batch, int max_len, int iters) {
  // Compute prefix sums for offsets
  std::vector<size_t> offsets(static_cast<size_t>(batch) + 1);
  offsets[0] = 0;
  for (int b = 0; b < batch; ++b) {
    offsets[static_cast<size_t>(b) + 1] = offsets[static_cast<size_t>(b)] + 
                                          static_cast<size_t>(lengths[static_cast<size_t>(b)]);
  }
  const size_t total_packed = offsets[static_cast<size_t>(batch)];

  // Pre-allocate packed arrays once
  std::vector<uint32_t> packed_tokens(total_packed);
  std::vector<uint32_t> positions(total_packed);

  const uint32_t* __restrict__ src = padded_tokens.data();
  const int* __restrict__ lens = lengths.data();
  const size_t* __restrict__ offs = offsets.data();
  
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint32_t* __restrict__ dst_tok = packed_tokens.data();
    uint32_t* __restrict__ dst_pos = positions.data();

    // Pack tokens and positions
    for (int b = 0; b < batch; ++b) {
      const int len = lens[b];
      const size_t src_base = static_cast<size_t>(b) * static_cast<size_t>(max_len);
      const size_t dst_base = offs[b];
      
      for (int i = 0; i < len; ++i) {
        dst_tok[dst_base + static_cast<size_t>(i)] = src[src_base + static_cast<size_t>(i)];
        dst_pos[dst_base + static_cast<size_t>(i)] = static_cast<uint32_t>(i);
      }
    }

    // Compute hash
    hash = 1469598103934665603ULL;
    for (size_t i = 0; i < total_packed; ++i) {
      hash = mix(hash, dst_tok[i]);
      hash = mix(hash, dst_pos[i]);
    }
  }

  return hash;
}