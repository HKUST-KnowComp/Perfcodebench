#include "interface.h"
#include <string>

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
  const size_t size = text.size();

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    size_t start = 0;
    while (start < size) {
      const char* ptr = data + start;
      const char* next_nl = static_cast<const char*>(__builtin_memchr(ptr, '\n', size - start));
      
      size_t end = (next_nl == nullptr) ? size : (next_nl - data);
      size_t line_len = end - start;
      
      hash = mix(hash, line_len);
      hash = mix(hash, (line_len == 0) ? 0u : static_cast<unsigned char>(data[start]));
      
      start = end + 1;
    }
    // Handle trailing newline case if the string ends with '\n'
    if (size > 0 && data[size - 1] == '\n') {
        hash = mix(hash, 0);
        hash = mix(hash, 0);
    }
  }
  return hash;
}