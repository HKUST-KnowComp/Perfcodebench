#include "interface.h"
#include <vector>

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  if (iters <= 0) return 0;

  struct Entry {
    uint32_t len;
    uint8_t first;
  };

  std::vector<Entry> pre(dict.size());
  for (size_t i = 0; i < dict.size(); ++i) {
    pre[i].len = static_cast<uint32_t>(dict[i].size());
    pre[i].first = dict[i].empty() ? 0 : static_cast<uint8_t>(dict[i][0]);
  }

  const uint64_t prime = 1099511628211ULL;
  uint64_t hash = 1469598103934665603ULL;

  const Entry* pre_ptr = pre.data();
  const uint32_t* codes_ptr = codes.data();
  const size_t codes_size = codes.size();

  for (size_t i = 0; i < codes_size; ++i) {
    const Entry& e = pre_ptr[codes_ptr[i]];
    hash ^= e.len;
    hash *= prime;
    hash ^= e.first;
    hash *= prime;
  }

  return hash;
}
