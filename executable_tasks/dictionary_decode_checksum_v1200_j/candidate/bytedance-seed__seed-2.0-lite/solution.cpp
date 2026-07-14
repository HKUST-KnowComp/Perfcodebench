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
  std::vector<std::pair<uint64_t, uint64_t>> preprocessed;
  preprocessed.reserve(codes.size());
  for (uint32_t code : codes) {
    const std::string& token = dict[static_cast<std::size_t>(code)];
    const uint64_t len = static_cast<uint64_t>(token.size());
    const uint64_t first = token.empty() ? 0ULL : static_cast<unsigned char>(token[0]);
    preprocessed.emplace_back(len, first);
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (const auto& entry : preprocessed) {
      hash = mix(hash, entry.first);
      hash = mix(hash, entry.second);
    }
  }
  return hash;
}