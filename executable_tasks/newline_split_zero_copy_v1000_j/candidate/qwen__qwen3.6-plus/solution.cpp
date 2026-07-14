#include "interface.h"
#include <cstdint>
#include <cstring>
#include <string>

uint64_t newline_split_checksum(const std::string& text, int iters) {
  uint64_t hash = 0;
  const char* data = text.data();
  const std::size_t n = text.size();
  constexpr uint64_t FNV_PRIME = 1099511628211ULL;
  constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;

  for (int iter = 0; iter < iters; ++iter) {
    hash = FNV_OFFSET;
    const char* cur = data;
    const char* end = data + n;
    while (cur < end) {
      const char* nl = static_cast<const char*>(std::memchr(cur, '\n', static_cast<std::size_t>(end - cur)));
      std::size_t len = nl ? static_cast<std::size_t>(nl - cur) : static_cast<std::size_t>(end - cur);
      
      hash ^= len;
      hash *= FNV_PRIME;
      
      if (len > 0) {
        hash ^= static_cast<unsigned char>(cur[0]);
      }
      hash *= FNV_PRIME;
      
      cur = nl ? nl + 1 : end;
    }
  }
  return hash;
}
