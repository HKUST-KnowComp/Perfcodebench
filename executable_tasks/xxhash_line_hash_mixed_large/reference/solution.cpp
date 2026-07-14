#include "interface.h"

#include "xxhash.h"

#include <cstring>

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  const char* p = input.data();
  const char* const end = p + input.size();
  while (p < end) {
    const void* newline = std::memchr(p, '\n', static_cast<std::size_t>(end - p));
    const char* line_end = newline ? static_cast<const char*>(newline) : end;
    acc ^= static_cast<uint64_t>(XXH64(p, static_cast<std::size_t>(line_end - p), 0));
    acc *= 1099511628211ULL;
    p = (line_end < end) ? (line_end + 1) : line_end;
  }
  return acc;
}
