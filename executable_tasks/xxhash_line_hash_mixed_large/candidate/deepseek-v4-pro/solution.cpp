#include "interface.h"

#include "xxhash.h"

#include <cstring>

uint64_t hash_lines(const std::string& input) {
  constexpr uint64_t PRIME = 1099511628211ULL;
  constexpr uint64_t EMPTY_HASH = 0xef46db3751d8e999ULL;  // XXH64("", 0, 0)
  uint64_t acc = 1469598103934665603ULL;
  const char* p = input.data();
  const char* const end = p + input.size();
  XXH64_state_t state;
  while (p < end) {
    const char* nl = static_cast<const char*>(std::memchr(p, '\n', end - p));
    const size_t len = (nl ? nl : end) - p;
    uint64_t h;
    if (__builtin_expect(len == 0, 0)) {
      h = EMPTY_HASH;
    } else {
      XXH64_reset(&state, 0);
      XXH64_update(&state, p, len);
      h = XXH64_digest(&state);
    }
    acc ^= h;
    acc *= PRIME;
    p = nl ? nl + 1 : end;
  }
  return acc;
}
