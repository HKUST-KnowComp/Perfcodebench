#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  if (iters <= 0) return 0;

  // Precompute size and first byte to avoid std::string overhead and pointer chasing
  struct Entry {
    uint64_t sz;
    uint64_t fb;
  };

  std::vector<Entry> info(dict.size());
  for (size_t i = 0; i < dict.size(); ++i) {
    info[i].sz = dict[i].size();
    info[i].fb = dict[i].empty() ? 0u : static_cast<unsigned char>(dict[i][0]);
  }

  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  const Entry* info_ptr = info.data();
  const uint32_t* codes_ptr = codes.data();
  const size_t codes_len = codes.size();

  for (size_t i = 0; i < codes_len; ++i) {
    const Entry& e = info_ptr[codes_ptr[i]];
    hash ^= e.sz;
    hash *= prime;
    hash ^= e.fb;
    hash *= prime;
  }

  return hash;
}