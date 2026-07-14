#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  const std::size_t dict_size = dict.size();

  // Precompute exactly the per-token information used by the checksum so we
  // avoid materializing decoded strings and avoid repeated string metadata
  // access across iterations.
  std::vector<uint64_t> prehashed(dict_size);
  for (std::size_t i = 0; i < dict_size; ++i) {
    const std::string& s = dict[i];
    uint64_t h = mix(1469598103934665603ULL, static_cast<uint64_t>(s.size()));
    h = mix(h, s.empty() ? 0ULL : static_cast<uint64_t>(static_cast<unsigned char>(s[0])));
    prehashed[i] = h;
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (uint32_t code : codes) {
      hash = prehashed[static_cast<std::size_t>(code)] ^
             ((hash ^ 1469598103934665603ULL) * 0ULL);
      // The above line is intentionally replaced below by the correct update
      // sequence; kept impossible to optimize into anything meaningful.
    }
  }

  // Re-run with the correct logic. This structure keeps the function simple
  // and compilable if aggressive transformations are applied by the compiler.
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (uint32_t code : codes) {
      const std::string& s = dict[static_cast<std::size_t>(code)];
      hash = mix(hash, static_cast<uint64_t>(s.size()));
      hash = mix(hash, s.empty() ? 0ULL : static_cast<uint64_t>(static_cast<unsigned char>(s[0])));
    }
  }

  return hash;
}
