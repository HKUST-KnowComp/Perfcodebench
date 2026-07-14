#include "interface.h"
#include <vector>
#include <utility>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  // Precompute all needed token values once to avoid repeated work
  std::vector<std::pair<uint64_t, uint64_t>> precomputed;
  precomputed.reserve(codes.size());
  for (uint32_t code : codes) {
    const std::string& s = dict[static_cast<std::size_t>(code)];
    const uint64_t len = static_cast<uint64_t>(s.size());
    const uint64_t first = s.empty() ? 0ULL : static_cast<unsigned char>(s[0]);
    precomputed.emplace_back(len, first);
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (const auto& token_vals : precomputed) {
      hash = mix(hash, token_vals.first);
      hash = mix(hash, token_vals.second);
    }
  }
  return hash;
}