#include "interface.h"

#include <string>
#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t h, uint64_t v) {
  h ^= v;
  h *= 1099511628211ULL;
  return h;
}

}  // namespace

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  const std::size_t n = codes.size();
  uint64_t hash = 1469598103934665603ULL;

  for (int iter = 0; iter < iters; ++iter) {
    for (std::size_t i = 0; i < n; ++i) {
      const std::string& token = dict[static_cast<std::size_t>(codes[i])];
      const std::size_t len = token.size();
      hash = mix(hash, len);
      const unsigned char first = (len != 0) ? static_cast<unsigned char>(token[0]) : 0u;
      hash = mix(hash, first);
    }
    // At the end of each iteration we still need the hash value for the next iteration,
    // just like the reference implementation resets hash inside the outer loop.
    // The reference resets hash to FNV offset basis on each iter start;
    // we do the same by reinitialising here.
    if (iter != iters - 1) {
      hash = 1469598103934665603ULL;
    }
  }
  return hash;
}