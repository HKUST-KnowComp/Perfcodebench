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
                              int batch,
                              int max_len,
                              int iters) {
  const std::size_t bsz = static_cast<std::size_t>(batch);
  const std::size_t mlen = static_cast<std::size_t>(max_len);

  // Pre-compute row start offsets and total packed size
  std::vector<std::size_t> row_start(bsz + 1, 0);
  std::size_t total = 0;
  for (std::size_t b = 0; b < bsz; ++b) {
    total += static_cast<std::size_t>(lengths[b]);
    row_start[b + 1] = total;
  }

  // Pre-allocate once; we never clear inside the loop
  std::vector<uint32_t> packed_tokens(total);
  std::vector<uint32_t> positions(total);

  uint64_t hash = 1469598103934665603ULL;

  for (int iter = 0; iter < iters; ++iter) {
    // Single fused pass: pack and hash on the fly
    std::size_t out_idx = 0;
    for (std::size_t b = 0; b < bsz; ++b) {
      const std::size_t row_offset = b * mlen;
      const int len = lengths[b];
      for (int pos = 0; pos < len; ++pos) {
        const uint32_t tok = padded_tokens[row_offset + static_cast<std::size_t>(pos)];
        packed_tokens[out_idx] = tok;
        positions[out_idx] = static_cast<uint32_t>(pos);
        hash = mix(hash, tok);
        hash = mix(hash, static_cast<uint64_t>(pos));
        ++out_idx;
      }
    }
  }

  return hash;
}