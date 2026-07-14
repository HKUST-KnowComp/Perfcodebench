#include "interface.h"

#include <vector>
#include <string>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  // Precompute required token metadata once to avoid repeated cold-cache accesses to the huge dict
  struct TokenMeta {
    size_t length;
    unsigned char leading_byte;
  };
  std::vector<TokenMeta> dict_metadata(dict.size());
  for (size_t i = 0; i < dict.size(); ++i) {
    const std::string& s = dict[i];
    dict_metadata[i].length = s.size();
    dict_metadata[i].leading_byte = s.empty() ? 0u : static_cast<unsigned char>(s[0]);
  }

  uint64_t final_hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    final_hash = 1469598103934665603ULL;
    // Process codes directly with no intermediate string materialization
    for (uint32_t code : codes) {
      const TokenMeta& meta = dict_metadata[static_cast<size_t>(code)];
      final_hash = mix(final_hash, meta.length);
      final_hash = mix(final_hash, meta.leading_byte);
    }
  }
  return final_hash;
}