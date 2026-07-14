#include "interface.h"

#include "xxhash.h"

#include <cstdint>
#include <string>

uint64_t hash_lines(const std::string& input) {
  constexpr uint64_t kOffset = 1469598103934665603ULL;
  constexpr uint64_t kPrime = 1099511628211ULL;

  uint64_t acc = kOffset;

  const char* const data = input.data();
  const std::size_t n = input.size();
  std::size_t start = 0;

  while (start < n) {
    const char* const base = data + start;
    const void* nl_ptr = __builtin_memchr(base, '\n', n - start);
    const std::size_t len = nl_ptr
                                ? static_cast<std::size_t>(static_cast<const char*>(nl_ptr) - base)
                                : (n - start);

    acc ^= static_cast<uint64_t>(XXH64(base, len, 0));
    acc *= kPrime;

    start += len + (nl_ptr != nullptr ? 1u : 0u);
  }

  return acc;
}
