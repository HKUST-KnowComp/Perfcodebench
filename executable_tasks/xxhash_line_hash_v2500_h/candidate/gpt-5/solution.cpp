#include "interface.h"

#include "xxhash.h"

#include <string>

uint64_t hash_lines(const std::string& input) {
  const uint64_t kFNV_offset = 1469598103934665603ULL;
  const uint64_t kFNV_prime = 1099511628211ULL;

  uint64_t acc = kFNV_offset;
  const std::size_t n = input.size();
  if (n == 0) return acc;

  const char* data = input.data();

  // Precompute the hash of an empty buffer once to avoid repeated calls.
  static const uint64_t empty_hash = []() -> uint64_t {
    const char* s = "";
    return XXH64(s, 0, 0);
  }();

  std::size_t pos = 0;
  while (pos < n) {
    std::size_t e = input.find('\n', pos);
    std::size_t len = (e == std::string::npos) ? (n - pos) : (e - pos);

    uint64_t h = (len == 0) ? empty_hash : static_cast<uint64_t>(XXH64(data + pos, len, 0));
    acc ^= h;
    acc *= kFNV_prime;

    if (e == std::string::npos) break;
    pos = e + 1;
  }

  return acc;
}
