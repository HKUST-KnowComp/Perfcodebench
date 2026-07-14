#include "interface.h"

#include "xxhash.h"

#include <string>
#include <cstring>

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  const char* p = input.data();
  const char* end = p + input.size();

  while (p < end) {
    size_t remaining = static_cast<size_t>(end - p);
    const void* nl_ptr = std::memchr(p, '\n', remaining);
    const char* line_end = nl_ptr ? static_cast<const char*>(nl_ptr) : end;
    size_t len = static_cast<size_t>(line_end - p);

    acc ^= static_cast<uint64_t>(XXH64(p, len, 0));
    acc *= 1099511628211ULL;

    if (!nl_ptr) break;
    p = line_end + 1;
  }

  return acc;
}
