#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  struct Entry {
    uint64_t len;
    uint64_t first;
  };

  const size_t dict_size = dict.size();
  std::vector<Entry> cache(dict_size);
  for (size_t i = 0; i < dict_size; ++i) {
    cache[i].len = dict[i].size();
    cache[i].first = dict[i].empty() ? 0u : static_cast<unsigned char>(dict[i][0]);
  }

  uint64_t hash = 0;
  const uint64_t fnv_offset = 1469598103934665603ULL;
  const uint64_t fnv_prime = 1099511628211ULL;

  for (int iter = 0; iter < iters; ++iter) {
    hash = fnv_offset;
    for (uint32_t code : codes) {
      const Entry& e = cache[code];
      hash ^= e.len;
      hash *= fnv_prime;
      hash ^= e.first;
      hash *= fnv_prime;
    }
  }
  return hash;
}