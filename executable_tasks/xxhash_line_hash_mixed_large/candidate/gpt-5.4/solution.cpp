#include "interface.h"

#include "xxhash.h"

#include <cstdint>
#include <string>

uint64_t hash_lines(const std::string& input) {
  static constexpr uint64_t kOffsetBasis = 1469598103934665603ULL;
  static constexpr uint64_t kPrime = 1099511628211ULL;

  uint64_t acc = kOffsetBasis;
  const char* data = input.data();
  const std::size_t size = input.size();

  std::size_t pos = 0;
  while (pos < size) {
    const char* line = data + pos;
    const char* nl = static_cast<const char*>(__builtin_memchr(line, '\n', size - pos));

    std::size_t len;
    if (nl) {
      len = static_cast<std::size_t>(nl - line);
      pos += len + 1;
    } else {
      len = size - pos;
      pos = size;
    }

    acc ^= static_cast<uint64_t>(XXH64(line, len, 0));
    acc *= kPrime;
  }

  return acc;
}
