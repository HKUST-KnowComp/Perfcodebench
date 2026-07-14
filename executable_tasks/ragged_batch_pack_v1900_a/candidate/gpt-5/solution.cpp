#include "interface.h"

namespace {

static inline uint64_t mix64(uint64_t h, uint64_t v) {
  h ^= v;
  h *= 1099511628211ULL;  // FNV prime
  return h;
}

}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
  const uint32_t* tokens = padded_tokens.data();
  const int* lens = lengths.data();

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;  // FNV offset basis
    for (int b = 0; b < batch; ++b) {
      int len = lens[b];
      const uint32_t* row = tokens + static_cast<std::size_t>(b) * static_cast<std::size_t>(max_len);
      for (int i = 0; i < len; ++i) {
        h = mix64(h, static_cast<uint64_t>(row[i]));
        h = mix64(h, static_cast<uint32_t>(i));
      }
    }
    hash = h;  // match baseline behavior: keep the last iteration's hash
  }
  return hash;
}
