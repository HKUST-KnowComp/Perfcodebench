#include "interface.h"
#include "xxhash.h"

namespace {
constexpr std::size_t kChunkSize = 1u << 10; // 1024 bytes

inline uint64_t rol1(uint64_t x) noexcept {
  return (x << 1) | (x >> 63);
}
}  // namespace

uint64_t chunked_hash(const std::string& input) {
  const char* p = input.data();
  const std::size_t n = input.size();
  uint64_t acc = 0;

  if (n == 0) {
    return 0;
  }

  const char* end = p + n;
  const std::size_t n_full = n & ~(kChunkSize - 1); // round down to multiple of kChunkSize
  const char* full_end = p + n_full;

  // Process full chunks
  for (; p != full_end; p += kChunkSize) {
    const uint64_t h = static_cast<uint64_t>(XXH3_64bits(p, kChunkSize));
    acc ^= h;
    acc = rol1(acc);
  }

  // Process tail if any
  const std::size_t rem = static_cast<std::size_t>(end - p);
  if (rem) {
    const uint64_t h = static_cast<uint64_t>(XXH3_64bits(p, rem));
    acc ^= h;
    acc = rol1(acc);
  }

  return acc;
}
