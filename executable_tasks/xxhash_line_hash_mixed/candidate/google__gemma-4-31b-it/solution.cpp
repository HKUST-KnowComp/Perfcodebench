#include "interface.h"
#include "xxhash.h"
#include <cstring>

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  const char* p = input.data();
  const char* const end = p + input.size();
  const size_t total_len = input.size();
  size_t remaining = total_len;

  while (remaining > 0) {
    const char* line_end = static_cast<const char*>(std::memchr(p, '\n', remaining));
    if (line_end) {
      size_t len = line_end - p;
      acc ^= XXH64(p, len, 0);
      acc *= 1099511628211ULL;
      p = line_end + 1;
      remaining -= (len + 1);
    } else {
      acc ^= XXH64(p, remaining, 0);
      acc *= 1099511628211ULL;
      remaining = 0;
    }
  }
  return acc;
}