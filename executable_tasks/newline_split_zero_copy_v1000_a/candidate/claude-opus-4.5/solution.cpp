#include "interface.h"

#include <cstdint>
#include <cstring>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) noexcept {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
  const char* const data = text.data();
  const std::size_t len = text.size();
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    const char* ptr = data;
    const char* const end_ptr = data + len;
    
    while (ptr < end_ptr) {
      const char* nl = static_cast<const char*>(std::memchr(ptr, '\n', static_cast<std::size_t>(end_ptr - ptr)));
      std::size_t line_len;
      const char* next;
      if (nl) {
        line_len = static_cast<std::size_t>(nl - ptr);
        next = nl + 1;
      } else {
        line_len = static_cast<std::size_t>(end_ptr - ptr);
        next = end_ptr;
      }
      hash = mix(hash, line_len);
      hash = mix(hash, line_len == 0 ? 0u : static_cast<unsigned char>(*ptr));
      ptr = next;
    }
  }
  return hash;
}