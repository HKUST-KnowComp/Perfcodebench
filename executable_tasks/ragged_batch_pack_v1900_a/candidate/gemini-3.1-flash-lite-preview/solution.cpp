#include "interface.h"
#include <vector>
#include <numeric>

namespace {
inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
  size_t total_tokens = 0;
  for (int len : lengths) total_tokens += static_cast<size_t>(len);

  uint64_t final_hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    const uint32_t* src_ptr = padded_tokens.data();
    
    for (int b = 0; b < batch; ++b) {
      int len = lengths[b];
      const uint32_t* row_ptr = src_ptr + (static_cast<size_t>(b) * max_len);
      for (int i = 0; i < len; ++i) {
        hash = mix(hash, row_ptr[i]);
        hash = mix(hash, static_cast<uint32_t>(i));
      }
    }
    final_hash = hash;
  }
  return final_hash;
}