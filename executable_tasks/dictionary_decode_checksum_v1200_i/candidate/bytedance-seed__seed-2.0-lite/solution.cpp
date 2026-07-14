#include "interface.h"

#include <string>
#include <vector>
#include <utility>

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
  // Precompute required values from dictionary once for all iterations
  std::vector<std::pair<uint64_t, uint64_t>> precomputed(dict.size());
  for (size_t i = 0; i < dict.size(); ++i) {
    const std::string& token = dict[i];
    precomputed[i].first = token.size();
    precomputed[i].second = token.empty() ? 0ULL : static_cast<unsigned char>(token[0]);
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (uint32_t code : codes) {
      const auto& p = precomputed[static_cast<size_t>(code)];
      hash = mix(hash, p.first);
      hash = mix(hash, p.second);
    }
  }
  return hash;
}