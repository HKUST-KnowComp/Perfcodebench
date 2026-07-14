#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

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
  if (iters <= 0) return 0;

  const uint32_t* codes_data = codes.data();
  const std::string* dict_data = dict.data();

  uint64_t hash = 1469598103934665603ULL;
  const std::size_t n = codes.size();
  for (std::size_t i = 0; i < n; ++i) {
    const std::string& token = dict_data[static_cast<std::size_t>(codes_data[i])];
    const uint64_t len = static_cast<uint64_t>(token.size());
    const uint64_t first = token.empty() ? 0u : static_cast<unsigned char>(token[0]);
    hash = mix(hash, len);
    hash = mix(hash, first);
  }

  return hash;
}
