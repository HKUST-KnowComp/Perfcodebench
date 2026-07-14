#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

uint64_t mix(uint64_t h, uint64_t v) {
  h ^= v;
  h *= 1099511628211ULL;
  return h;
}

}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
  std::size_t total_tokens = 0;
  for (int b = 0; b < batch; ++b) {
    total_tokens += static_cast<std::size_t>(lengths[b]);
  }
  std::vector<uint32_t> packed_tokens(total_tokens);
  std::size_t idx = 0;
  for (int b = 0; b < batch; ++b) {
    int L = lengths[b];
    const uint32_t* src = &padded_tokens[static_cast<std::size_t>(b) * static_cast<std::size_t>(max_len)];
    for (int i = 0; i < L; ++i) {
      packed_tokens[idx++] = src[i];
    }
  }

  const uint64_t SEED = 1469598103934665603ULL;
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = SEED;
    std::size_t k = 0;
    for (int b = 0; b < batch; ++b) {
      int L = lengths[b];
      for (int i = 0; i < L; ++i) {
        hash = mix(hash, packed_tokens[k]);
        hash = mix(hash, static_cast<uint32_t>(i));
        ++k;
      }
    }
  }
  return hash;
}