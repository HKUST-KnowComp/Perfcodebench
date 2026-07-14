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

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
  uint64_t final_hash = 0;
  const size_t b_size = static_cast<size_t>(batch);
  const size_t m_len = static_cast<size_t>(max_len);

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t b = 0; b < b_size; ++b) {
      const int len = lengths[b];
      const uint32_t* row_ptr = &padded_tokens[b * m_len];
      for (int i = 0; i < len; ++i) {
        hash = mix(hash, row_ptr[i]);
        hash = mix(hash, static_cast<uint32_t>(i));
      }
    }
    final_hash = hash;
  }
  return final_hash;
}