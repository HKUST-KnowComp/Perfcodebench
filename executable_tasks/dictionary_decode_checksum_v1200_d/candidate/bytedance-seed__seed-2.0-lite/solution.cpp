#include "interface.h"

#include <string>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  // Precompute all required token values once to avoid repeated work
  std::vector<std::pair<uint64_t, uint64_t>> token_mix_values;
  token_mix_values.reserve(codes.size());
  for (uint32_t code : codes) {
    const std::string& token = dict[static_cast<std::size_t>(code)];
    const uint64_t size = token.size();
    const uint64_t leading = token.empty() ? 0u : static_cast<unsigned char>(token[0]);
    token_mix_values.emplace_back(size, leading);
  }

  uint64_t final_hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    for (const auto& p : token_mix_values) {
      hash = mix(hash, p.first);
      hash = mix(hash, p.second);
    }
    final_hash = hash;
  }
  return final_hash;
}