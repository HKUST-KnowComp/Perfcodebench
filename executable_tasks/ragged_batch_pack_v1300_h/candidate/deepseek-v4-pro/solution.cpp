#include "interface.h"

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens,
                              const std::vector<int>& lengths, int batch,
                              int max_len, int iters) {
  const uint64_t start_hash = 1469598103934665603ULL;
  const uint32_t* tokens_data = padded_tokens.data();
  const int* lengths_data = lengths.data();
  uint64_t hash;
  for (int iter = 0; iter < iters; ++iter) {
    hash = start_hash;
    for (int b = 0; b < batch; ++b) {
      const int len = lengths_data[b];
      const uint32_t* row = &tokens_data[static_cast<std::size_t>(b) * max_len];
      for (int i = 0; i < len; ++i) {
        hash = mix(hash, row[i]);
        hash = mix(hash, static_cast<uint32_t>(i));
      }
    }
  }
  return hash;
}
