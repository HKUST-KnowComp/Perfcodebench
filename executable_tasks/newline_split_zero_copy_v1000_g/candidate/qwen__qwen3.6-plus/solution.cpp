#include "interface.h"
#include <cstring>
#include <cstdint>

uint64_t newline_split_checksum(const std::string& text, int iters) {
  const char* data = text.data();
  const char* end_data = data + text.size();
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    const char* start = data;
    while (start < end_data) {
      const char* nl = static_cast<const char*>(std::memchr(start, '\n', end_data - start));
      size_t line_len = nl ? (nl - start) : (end_data - start);
      uint64_t first_char = (line_len > 0) ? static_cast<unsigned char>(start[0]) : 0u;

      hash ^= line_len;
      hash *= 1099511628211ULL;
      hash ^= first_char;
      hash *= 1099511628211ULL;

      start = nl ? nl + 1 : end_data;
    }
  }
  return hash;
}
