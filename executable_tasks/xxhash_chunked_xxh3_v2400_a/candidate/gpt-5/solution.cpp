#include "interface.h"

#include "xxhash.h"

#include <string>
#include <cstdint>

namespace {
constexpr std::size_t kChunkSize = 1u << 10; // 1024 bytes

inline uint64_t rotl1(uint64_t x) {
#if defined(__has_builtin)
  #if __has_builtin(__builtin_rotateleft64)
    return __builtin_rotateleft64(x, 1);
  #else
    return (x << 1) | (x >> 63);
  #endif
#else
  return (x << 1) | (x >> 63);
#endif
}
}

uint64_t chunked_hash(const std::string& input) {
  const std::size_t n = input.size();
  const char* base = input.data();
  uint64_t acc = 0;

  if (n == 0) return acc;

  const std::size_t full_chunks = n / kChunkSize;
  const std::size_t rem = n % kChunkSize;

  const char* p = base;

  // Process full chunks
  for (std::size_t i = 0; i < full_chunks; ++i) {
    const uint64_t h = XXH3_64bits(p, kChunkSize);
    acc ^= h;
    acc = rotl1(acc);
    p += kChunkSize;
  }

  // Process remainder chunk if any
  if (rem) {
    const uint64_t h = XXH3_64bits(p, rem);
    acc ^= h;
    acc = rotl1(acc);
  }

  return acc;
}
