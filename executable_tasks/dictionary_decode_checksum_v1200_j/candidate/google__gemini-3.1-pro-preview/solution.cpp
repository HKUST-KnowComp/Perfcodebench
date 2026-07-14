#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

struct TokenInfo {
  uint64_t size;
  uint64_t first_char;
};

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  if (iters <= 0) {
    return 0;
  }

  std::vector<TokenInfo> info(dict.size());
  for (std::size_t i = 0; i < dict.size(); ++i) {
    info[i].size = dict[i].size();
    info[i].first_char = dict[i].empty() ? 0u : static_cast<unsigned char>(dict[i][0]);
  }

  uint64_t hash = 1469598103934665603ULL;
  const uint32_t* codes_ptr = codes.data();
  std::size_t codes_size = codes.size();
  const TokenInfo* info_ptr = info.data();

  for (std::size_t i = 0; i < codes_size; ++i) {
    uint32_t code = codes_ptr[i];
    hash ^= info_ptr[code].size;
    hash *= 1099511628211ULL;
    hash ^= info_ptr[code].first_char;
    hash *= 1099511628211ULL;
  }

  return hash;
}
