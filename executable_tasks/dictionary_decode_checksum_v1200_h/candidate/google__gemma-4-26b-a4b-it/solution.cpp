#include "interface.h"

#include <string>
#include <vector>
#include <cstdint>

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
  uint64_t hash = 0;
  
  // Pre-calculate the hash components for each dictionary entry to avoid
  // repeated access and branching inside the hot loop.
  struct EntryInfo {
    uint64_t size;
    uint64_t first_byte;
  };
  
  std::vector<EntryInfo> info_cache;
  info_cache.reserve(dict.size());
  for (const auto& s : dict) {
    info_cache.push_back({
      static_cast<uint64_t>(s.size()),
      s.empty() ? 0ULL : static_cast<unsigned char>(s[0])
    });
  }

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (uint32_t code : codes) {
      const auto& info = info_cache[static_cast<std::size_t>(code)];
      hash = mix(hash, info.size);
      hash = mix(hash, info.first_byte);
    }
  }
  
  return hash;
}