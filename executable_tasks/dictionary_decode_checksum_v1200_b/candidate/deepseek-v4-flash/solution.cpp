#include "interface.h"

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  uint64_t result = 0;
  constexpr uint64_t kInitHash = 1469598103934665603ULL;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = kInitHash;
    for (const auto& code : codes) {
      const std::string& token = dict[code];
      // mix size
      hash ^= static_cast<uint64_t>(token.size());
      hash *= 1099511628211ULL;
      // mix first char
      hash ^= static_cast<uint64_t>(token.empty() ? 0u : static_cast<unsigned char>(token[0]));
      hash *= 1099511628211ULL;
    }
    result = hash;
  }
  return result;
}
