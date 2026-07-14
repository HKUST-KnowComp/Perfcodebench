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
  // Precompute only required metadata from dictionary once, avoid repeated string copies
  std::vector<std::pair<size_t, unsigned char>> dict_meta;
  dict_meta.reserve(dict.size());
  for (const auto& s : dict) {
    const size_t len = s.size();
    const unsigned char first_byte = len ? static_cast<unsigned char>(s[0]) : 0u;
    dict_meta.emplace_back(len, first_byte);
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (uint32_t code : codes) {
      const auto& meta = dict_meta[static_cast<std::size_t>(code)];
      hash = mix(hash, meta.first);
      hash = mix(hash, meta.second);
    }
  }
  return hash;
}