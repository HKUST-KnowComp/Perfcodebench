#include "interface.h"

#include <vector>
#include <cstring>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
  // Compute total packed length
  size_t total_len = 0;
  for (int b = 0; b < batch; ++b) {
    total_len += static_cast<size_t>(lengths[static_cast<size_t>(b)]);
  }
  
  // Pre-allocate packed_tokens once
  std::vector<uint32_t> packed_tokens(total_len);
  
  // Compute prefix sums for batch offsets
  std::vector<size_t> offsets(static_cast<size_t>(batch) + 1);
  offsets[0] = 0;
  for (int b = 0; b < batch; ++b) {
    offsets[static_cast<size_t>(b) + 1] = offsets[static_cast<size_t>(b)] + static_cast<size_t>(lengths[static_cast<size_t>(b)]);
  }
  
  uint64_t hash = 0;
  const size_t ml = static_cast<size_t>(max_len);
  
  for (int iter = 0; iter < iters; ++iter) {
    // Pack tokens using memcpy for each batch row
    for (int b = 0; b < batch; ++b) {
      const int len = lengths[static_cast<size_t>(b)];
      if (len > 0) {
        std::memcpy(&packed_tokens[offsets[static_cast<size_t>(b)]],
                    &padded_tokens[static_cast<size_t>(b) * ml],
                    static_cast<size_t>(len) * sizeof(uint32_t));
      }
    }
    
    // Compute hash - generate positions on the fly
    hash = 1469598103934665603ULL;
    for (int b = 0; b < batch; ++b) {
      const int len = lengths[static_cast<size_t>(b)];
      const size_t base = offsets[static_cast<size_t>(b)];
      for (int i = 0; i < len; ++i) {
        hash = mix(hash, packed_tokens[base + static_cast<size_t>(i)]);
        hash = mix(hash, static_cast<uint32_t>(i));
      }
    }
  }
  return hash;
}