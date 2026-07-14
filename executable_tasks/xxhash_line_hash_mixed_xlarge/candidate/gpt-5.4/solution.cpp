#include "interface.h"

#include "xxhash.h"

#include <cstdint>
#include <cstring>
#include <string>

uint64_t hash_lines(const std::string& input) {
  constexpr uint64_t kOffsetBasis = 1469598103934665603ULL;
  constexpr uint64_t kPrime = 1099511628211ULL;

  uint64_t acc = kOffsetBasis;
  const char* p = input.data();
  const char* const end = p + input.size();

  while (p < end) {
    const std::size_t remaining = static_cast<std::size_t>(end - p);
    const void* nl_ptr = std::memchr(p, '\n', remaining);
    const char* line_end = nl_ptr ? static_cast<const char*>(nl_ptr) : end;

    acc ^= static_cast<uint64_t>(XXH64(p, static_cast<std::size_t>(line_end - p), 0));
    acc *= kPrime;

    p = (line_end < end) ? (line_end + 1) : end;
  }

  return acc;
}
