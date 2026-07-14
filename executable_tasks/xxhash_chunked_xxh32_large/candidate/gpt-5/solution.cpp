#include "interface.h"
#include "xxhash.h"

namespace {
constexpr std::size_t kChunkSize = 1u << 10; // 1024 bytes

static inline uint64_t rotl1(uint64_t x) {
#if defined(__has_builtin)
#  if __has_builtin(__builtin_rotateleft64)
    return __builtin_rotateleft64(x, 1);
#  else
    return (x << 1) | (x >> 63);
#  endif
#elif defined(__clang__) || defined(__GNUC__)
    return __builtin_rotateleft64(x, 1);
#else
    return (x << 1) | (x >> 63);
#endif
}
}

uint64_t chunked_hash(const std::string& input) {
  const std::size_t n = input.size();
  const char* p = input.data();

  uint64_t acc = 0;

  // kChunkSize is 1024 (1<<10), use shifts/masks for speed
  std::size_t full = n >> 10;                 // number of full 1 KiB chunks
  const std::size_t rem = n & (kChunkSize - 1); // remaining bytes

  // Process full chunks with a small unroll
  while (full >= 4) {
    uint32_t h0 = XXH32(p, kChunkSize, 0); p += kChunkSize; acc ^= (uint64_t)h0; acc = rotl1(acc);
    uint32_t h1 = XXH32(p, kChunkSize, 0); p += kChunkSize; acc ^= (uint64_t)h1; acc = rotl1(acc);
    uint32_t h2 = XXH32(p, kChunkSize, 0); p += kChunkSize; acc ^= (uint64_t)h2; acc = rotl1(acc);
    uint32_t h3 = XXH32(p, kChunkSize, 0); p += kChunkSize; acc ^= (uint64_t)h3; acc = rotl1(acc);
    full -= 4;
  }

  while (full) {
    uint32_t h = XXH32(p, kChunkSize, 0);
    p += kChunkSize;
    acc ^= (uint64_t)h;
    acc = rotl1(acc);
    --full;
  }

  if (rem) {
    uint32_t h = XXH32(p, rem, 0);
    acc ^= (uint64_t)h;
    acc = rotl1(acc);
  }

  return acc;
}
