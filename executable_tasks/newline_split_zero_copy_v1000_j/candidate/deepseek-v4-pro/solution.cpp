#include "interface.h"
#include <cstdint>
#include <string>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
  const char* const data = text.data();
  const std::size_t size = text.size();
  const uint64_t seed = 1469598103934665603ULL;
  uint64_t final_hash = 0;
  for (int i = 0; i < iters; ++i) {
    uint64_t hash = seed;
    const char* p = data;
    const char* const end = data + size;
    while (p < end) {
      const char* line_start = p;
      while (p < end && *p != '\n') ++p;
      const std::size_t len = p - line_start;
      const unsigned char first = len ? static_cast<unsigned char>(*line_start) : 0;
      hash = mix(hash, len);
      hash = mix(hash, first);
      if (p < end) ++p;
    }
    final_hash = hash;
  }
  return final_hash;
}