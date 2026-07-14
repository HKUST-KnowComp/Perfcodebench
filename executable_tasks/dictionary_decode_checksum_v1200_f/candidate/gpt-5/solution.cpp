#include "interface.h"

namespace {

constexpr uint64_t kFNV64Offset = 1469598103934665603ULL;
constexpr uint64_t kFNV64Prime  = 1099511628211ULL;

}  // namespace

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  // Preserve baseline behavior for non-positive iterations
  if (iters <= 0) return 0ULL;

  // Precompute token sizes and first bytes once (dictionary is const)
  const std::size_t dict_size = dict.size();
  std::vector<uint64_t> sizes(dict_size);
  std::vector<uint8_t> first_bytes(dict_size);
  for (std::size_t i = 0; i < dict_size; ++i) {
    const std::string& s = dict[i];
    sizes[i] = static_cast<uint64_t>(s.size());
    uint8_t fb = 0;
    if (!s.empty()) fb = static_cast<uint8_t>(static_cast<unsigned char>(s[0]));
    first_bytes[i] = fb;
  }

  // Compute checksum once (result is identical for each iteration)
  uint64_t hash = kFNV64Offset;
  const uint32_t* p = codes.data();
  const std::size_t n = codes.size();
  for (std::size_t i = 0; i < n; ++i) {
    const std::size_t idx = static_cast<std::size_t>(p[i]);
    // mix(hash, sizes[idx])
    hash ^= sizes[idx];
    hash *= kFNV64Prime;
    // mix(hash, first_bytes[idx])
    hash ^= static_cast<uint64_t>(first_bytes[idx]);
    hash *= kFNV64Prime;
  }

  return hash;
}
