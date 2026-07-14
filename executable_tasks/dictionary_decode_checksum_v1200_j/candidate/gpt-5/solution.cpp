#include "interface.h"

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  return (hash ^ value) * 1099511628211ULL;
}

}  // namespace

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  if (iters <= 0) return 0ULL;

  const std::string* dict_ptr = dict.data();
  const uint32_t* codes_ptr = codes.data();
  const std::size_t n = codes.size();

  uint64_t hash = 1469598103934665603ULL;  // FNV-1a offset basis
  for (std::size_t i = 0; i < n; ++i) {
    const std::string& token = dict_ptr[static_cast<std::size_t>(codes_ptr[i])];
    const uint64_t len = static_cast<uint64_t>(token.size());
    const uint64_t lead = token.empty() ? 0u : static_cast<unsigned char>(token[0]);
    hash = mix(hash, len);
    hash = mix(hash, lead);
  }
  return hash;
}
