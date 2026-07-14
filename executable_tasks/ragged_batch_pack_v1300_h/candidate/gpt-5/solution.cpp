#include "interface.h"

#include <cstddef>

namespace {
static inline uint64_t fnv_mix(uint64_t h, uint64_t v) {
  // FNV-1a step used in baseline mix: hash ^= value; hash *= PRIME
  static constexpr uint64_t PRIME = 1099511628211ULL;
  h ^= v;
  h *= PRIME;
  return h;
}
}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
  // Stream the packed data directly into the hash to avoid building temporary vectors.
  const uint32_t* data = padded_tokens.data();
  const int* lens = lengths.data();
  const int B = batch;
  const int L = max_len;

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Reset hash each iteration as in baseline.
    hash = 1469598103934665603ULL;  // FNV offset basis

    const uint32_t* row = data;
    for (int b = 0; b < B; ++b) {
      int len = lens[b];
      if (len > 0) {
        // Process tokens in this row up to len (no bounds checks added to preserve baseline behavior)
        int i = 0;
        // Small unroll for throughput
        for (; i + 4 <= len; i += 4) {
          // i
          hash = fnv_mix(hash, static_cast<uint64_t>(row[i]));
          hash = fnv_mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(i)));
          // i+1
          hash = fnv_mix(hash, static_cast<uint64_t>(row[i + 1]));
          hash = fnv_mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(i + 1)));
          // i+2
          hash = fnv_mix(hash, static_cast<uint64_t>(row[i + 2]));
          hash = fnv_mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(i + 2)));
          // i+3
          hash = fnv_mix(hash, static_cast<uint64_t>(row[i + 3]));
          hash = fnv_mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(i + 3)));
        }
        for (; i < len; ++i) {
          hash = fnv_mix(hash, static_cast<uint64_t>(row[i]));
          hash = fnv_mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(i)));
        }
      }
      row += static_cast<std::size_t>(L);
    }
  }

  return hash;
}
