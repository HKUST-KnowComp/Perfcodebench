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

struct DictMeta {
  std::size_t len;
  unsigned char first;
};

}  // namespace

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  std::vector<DictMeta> meta;
  meta.reserve(dict.size());
  for (const std::string& s : dict) {
    meta.push_back(DictMeta{s.size(), s.empty() ? static_cast<unsigned char>(0)
                                                : static_cast<unsigned char>(s[0])});
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;
    for (uint32_t code : codes) {
      const DictMeta& m = meta[static_cast<std::size_t>(code)];
      h ^= static_cast<uint64_t>(m.len);
      h *= 1099511628211ULL;
      h ^= static_cast<uint64_t>(m.first);
      h *= 1099511628211ULL;
    }
    hash = h;
  }
  return hash;
}
