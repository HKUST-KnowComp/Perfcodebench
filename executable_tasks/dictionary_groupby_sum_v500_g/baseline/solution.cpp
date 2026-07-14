#include "interface.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t dictionary_groupby_checksum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& values,
    int iters) {
  uint64_t hash = 0;
  std::vector<std::string> decoded;
  std::unordered_map<std::string, uint64_t> sums;
  for (int iter = 0; iter < iters; ++iter) {
    decoded.resize(codes.size());
    for (std::size_t i = 0; i < codes.size(); ++i) {
      decoded[i] = dictionary[static_cast<std::size_t>(codes[i])];
    }
    sums.clear();
    for (std::size_t i = 0; i < decoded.size(); ++i) {
      sums[decoded[i]] += static_cast<uint64_t>(values[i]);
    }
    hash = 1469598103934665603ULL;
    for (const std::string& key : dictionary) {
      hash = mix(hash, sums[key]);
    }
  }
  return hash;
}
