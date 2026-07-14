#include "interface.h"
#include "xxhash.h"
#include <string_view>
#include <cstring>

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  std::string_view data(input);
  const char* p = data.data();
  const char* end = p + data.size();

  while (p < end) {
    const char* line_end = static_cast<const char*>(std::memchr(p, '\n', end - p));
    size_t len = line_end ? (line_end - p) : (end - p);
    
    acc ^= static_cast<uint64_t>(XXH64(p, len, 0));
    acc *= 1099511628211ULL;
    
    if (!line_end) break;
    p = line_end + 1;
  }
  return acc;
}