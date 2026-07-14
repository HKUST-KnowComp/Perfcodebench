#include "interface.h"
#include "xxhash.h"

namespace {
constexpr std::size_t kChunkSize = 1u << 10; // 1024 bytes

static inline uint64_t rotl1(uint64_t x) {
#if defined(__has_builtin)
#  if __has_builtin(__builtin_rotateleft64)
    return __builtin_rotateleft64(x, 1);
#  endif
#endif
    return (x << 1) | (x >> 63);
}
}

uint64_t chunked_hash(const std::string& input) {
  const std::size_t size = input.size();
  uint64_t acc = 0;
  if (size == 0) return acc;

  const char* p = input.data();

  const std::size_t full = size / kChunkSize;
  for (std::size_t i = 0; i < full; ++i) {
    const XXH64_hash_t h = XXH3_64bits(p, kChunkSize);
    acc ^= static_cast<uint64_t>(h);
    acc = rotl1(acc);
    p += kChunkSize;
  }

  const std::size_t tail = size - full * kChunkSize;
  if (tail) {
    const XXH64_hash_t h = XXH3_64bits(p, tail);
    acc ^= static_cast<uint64_t>(h);
    acc = rotl1(acc);
  }

  return acc;
}
