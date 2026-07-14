#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  
  struct Entry {
    uint64_t len;
    uint8_t first;
  };
  std::vector<Entry> entries(dict.size());
  for (size_t i = 0; i < dict.size(); ++i) {
    entries[i].len = dict[i].size();
    entries[i].first = dict[i].empty() ? 0 : static_cast<uint8_t>(dict[i][0]);
  }

  uint64_t hash = 0;
  const uint64_t prime = 1099511628211ULL;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;
    for (uint32_t code : codes) {
      const Entry& e = entries[code];
      h ^= e.len;
      h *= prime;
      h ^= e.first;
      h *= prime;
    }
    hash = h;
  }
  return hash;
}
