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

constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;

}  // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
  if (iters <= 0) return 0;

  const char* const data = text.data();
  const size_t size = text.size();
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    hash = FNV_OFFSET_BASIS;
    
    // The baseline uses: while (start < text.size())
    // This means if text is empty, the loop never runs and hash remains FNV_OFFSET_BASIS.
    // If text is not empty, it processes segments separated by '\n'.
    size_t start = 0;
    while (start < size) {
      const char* current_ptr = data + start;
      const char* next_nl = static_cast<const char*>(std::memchr(current_ptr, '\n', size - start));
      
      size_t line_len;
      if (next_nl) {
        line_len = next_nl - current_ptr;
        start += line_len + 1;
      } else {
        line_len = size - start;
        start = size; // Terminate loop
      }

      // Checksum logic: mix length, then mix first character (or 0 if empty)
      hash = mix(hash, static_cast<uint64_t>(line_len));
      uint64_t first_char = (line_len == 0) ? 0u : static_cast<unsigned char>(*current_ptr);
      hash = mix(hash, first_char);
    }
  }

  return hash;
}