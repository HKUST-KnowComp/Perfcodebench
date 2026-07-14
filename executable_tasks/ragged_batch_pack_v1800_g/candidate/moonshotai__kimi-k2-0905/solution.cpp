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
                              const std::vector<int>&      lengths,
                              int batch, int max_len, int iters) {
  // Fast-path: nothing to do
  if (batch == 0 || iters == 0) return 0;

  // Pre-compute total length so we never re-allocate
  std::size_t total_len = 0;
  for (int b = 0; b < batch; ++b) total_len += static_cast<std::size_t>(lengths[b]);

  // Re-usable scratch space; reserve only once
  thread_local std::vector<uint32_t> scratch;
  scratch.clear();
  scratch.reserve(total_len * 2); // tokens + positions

  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Directly stream into scratch (no per-iteration allocations)
    uint32_t* token_out  = scratch.data();
    uint32_t* pos_out    = scratch.data() + total_len;

    const uint32_t* src = padded_tokens.data();
    for (int b = 0; b < batch; ++b) {
      int len = lengths[b];
      for (int i = 0; i < len; ++i) {
        *token_out++ = src[i];
        *pos_out++   = static_cast<uint32_t>(i);
      }
      src += max_len;
    }

    // Hash on-the-fly
    uint64_t hash = 1469598103934665603ULL;
    const uint32_t* tk = scratch.data();
    const uint32_t* ps = scratch.data() + total_len;
    for (std::size_t k = 0; k < total_len; ++k) {
      hash = mix(hash, tk[k]);
      hash = mix(hash, ps[k]);
    }
    final_hash = hash;
  }

  return final_hash;
}