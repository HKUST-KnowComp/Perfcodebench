#include "interface.h"

#include <cstdint>
#include <string>

#include "xxhash.h"

uint64_t hash_lines(const std::string& input) {
  constexpr uint64_t kOffset = 1469598103934665603ULL;
  constexpr uint64_t kPrime = 1099511628211ULL;

  uint64_t acc = kOffset;
  const char* data = input.data();
  const std::size_t size = input.size();

  std::size_t start = 0;
  while (start < size) {
    const void* nl_ptr = __builtin_memchr(data + start, '\n', size - start);
    std::size_t len;
    if (nl_ptr) {
      const char* nl = static_cast<const char*>(nl_ptr);
      len = static_cast<std::size_t>(nl - (data + start));
      start += len + 1;
    } else {
      len = size - start;
      start = size;
    }

    acc ^= static_cast<uint64_t>(XXH64(data + (start - (nl_ptr ? (len + 1) : len)), len, 0));
    acc *= kPrime;
  }

  return acc;
}
