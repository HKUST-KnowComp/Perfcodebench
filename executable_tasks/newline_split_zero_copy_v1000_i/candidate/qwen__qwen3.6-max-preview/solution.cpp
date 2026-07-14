#include "interface.h"
#include <string>
#include <cstring>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
  uint64_t hash = 0;
  const char* data = text.data();
  const char* end_ptr = data + text.size();

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    const char* p = data;
    while (p < end_ptr) {
      const char* nl = static_cast<const char*>(std::memchr(p, '\n', end_ptr - p));
      const char* line_end = nl ? nl : end_ptr;
      std::size_t line_len = line_end - p;
      uint64_t first_char = (line_len == 0) ? 0u : static_cast<unsigned char>(*p);
      
      hash = mix(hash, line_len);
      hash = mix(hash, first_char);
      
      p = line_end + 1;
    }
  }
  return hash;
}