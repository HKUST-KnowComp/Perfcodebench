#include "interface.h"
#include <cstdint>
#include <cstddef>

namespace {
uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens,
                               const std::vector<int>& lengths,
                               int batch, int max_len, int iters) {
  const uint32_t* tokens_ptr = padded_tokens.data();
  const int* lengths_ptr = lengths.data();
  const std::size_t max_len_s = static_cast<std::size_t>(max_len);
  const int batch_cnt = batch;
  const int iters_cnt = iters;
  constexpr uint64_t init_hash = 1469598103934665603ULL;

  uint64_t hash = init_hash;
  for (int iter = 0; iter < iters_cnt; ++iter) {
    hash = init_hash;
    for (std::size_t b = 0; b < static_cast<std::size_t>(batch_cnt); ++b) {
      const int len = lengths_ptr[b];
      const uint32_t* row = tokens_ptr + b * max_len_s;
      for (int i = 0; i < len; ++i) {
        hash = mix(hash, static_cast<uint64_t>(row[i]));
        hash = mix(hash, static_cast<uint32_t>(i));
      }
    }
  }
  return hash;
}
