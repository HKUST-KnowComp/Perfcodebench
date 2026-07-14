#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  const std::size_t dict_size = dict.size();

  std::vector<uint64_t> lens(dict_size);
  std::vector<uint64_t> firsts(dict_size);
  for (std::size_t i = 0; i < dict_size; ++i) {
    const std::string& s = dict[i];
    lens[i] = static_cast<uint64_t>(s.size());
    firsts[i] = s.empty() ? 0ULL : static_cast<unsigned char>(s[0]);
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (uint32_t code : codes) {
      const std::size_t idx = static_cast<std::size_t>(code);
      hash = mix(hash, lens[idx]);
      hash = mix(hash, firsts[idx]);
    }
  }
  return hash;
}
