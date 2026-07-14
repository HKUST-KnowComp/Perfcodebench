#include "interface.h"

#include <string>

static inline uint64_t mix_inline(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

uint64_t newline_split_checksum(const std::string& text, int iters) {
  const char* data = text.data();
  const std::size_t n = text.size();

  uint64_t result = 0;  // Matches baseline behavior when iters == 0
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;

    const char* p = data;
    const char* end = data + n;
    while (p < end) {
      const char* s = p;
      // Scan to next newline or end
      while (p < end) {
        char c = *p;
        if (c == '\n') break;
        ++p;
      }
      std::size_t len = static_cast<std::size_t>(p - s);
      uint64_t fc = (len != 0) ? static_cast<unsigned char>(*s) : 0u;
      hash = mix_inline(hash, len);
      hash = mix_inline(hash, fc);

      if (p < end) {
        ++p;  // skip the newline
      } else {
        break;
      }
    }

    result = hash;  // return the checksum from the last iteration
  }

  return result;
}
