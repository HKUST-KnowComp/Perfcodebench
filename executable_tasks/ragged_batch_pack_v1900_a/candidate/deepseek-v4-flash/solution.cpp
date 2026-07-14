#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

constexpr uint64_t kOffsetBasis = 1469598103934665603ULL;
constexpr uint64_t kPrime = 1099511628211ULL;

inline uint64_t mix(uint64_t hash, uint64_t value) noexcept {
  hash ^= value;
  hash *= kPrime;
  return hash;
}

}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens,
                              const std::vector<int>& lengths,
                              int batch,
                              int max_len,
                              int iters) {
  // Directly compute the hash without allocating intermediate vectors.
  uint64_t hash = kOffsetBasis;
  const uint32_t* tokens = padded_tokens.data();
  const int* lens = lengths.data();
  for (int iter = 0; iter < iters; ++iter) {
    hash = kOffsetBasis;
    for (int b = 0; b < batch; ++b) {
      const int len = lens[b];
      const uint32_t* row = tokens + static_cast<std::size_t>(b) * static_cast<std::size_t>(max_len);
      for (int i = 0; i < len; ++i) {
        hash = mix(hash, static_cast<uint64_t>(row[i]));
        hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(i)));
      }
    }
  }
  return hash;
}