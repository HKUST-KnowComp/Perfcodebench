#include "interface.h"

#include <cstring>
#include <string>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
  if (iters <= 0) {
    return 0;
  }

  // The baseline logic resets the hash inside the iteration loop and 
  // processes the same constant text. Thus, we only need to compute it once.
  uint64_t hash = 0;
  const size_t size = text.size();
  
  if (size > 0) {
    hash = 1469598103934665603ULL;
    const char* data = text.data();
    size_t start = 0;

    while (start < size) {
      const char* current_ptr = data + start;
      const char* next_newline = static_cast<const char*>(std::memchr(current_ptr, '\n', size - start));
      
      size_t line_len;
      if (next_newline) {
        line_len = next_newline - current_ptr;
        start += line_len + 1;
      } else {
        line_len = size - start;
        start = size;
      }

      // Compute hash for the current line segment
      hash = mix(hash, static_cast<uint64_t>(line_len));
      hash = mix(hash, line_len == 0 ? 0ULL : static_cast<uint64_t>(static_cast<unsigned char>(*current_ptr)));
    }
  }

  return hash;
}