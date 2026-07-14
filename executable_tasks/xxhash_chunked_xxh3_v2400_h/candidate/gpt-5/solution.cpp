#include "interface.h"
#include "xxhash.h"

namespace {
constexpr std::size_t kChunkSize = 1024;
inline uint64_t rotl1(uint64_t x) { return (x << 1) | (x >> 63); }
}

uint64_t chunked_hash(const std::string& input) {
  const std::size_t n = input.size();
  const char* p = input.data();
  uint64_t acc = 0;

  if (n == 0) return 0;

  const std::size_t fullChunks = n / kChunkSize;
  for (std::size_t i = 0; i < fullChunks; ++i) {
    acc ^= static_cast<uint64_t>(XXH3_64bits(p, kChunkSize));
    acc = rotl1(acc);
    p += kChunkSize;
  }

  const std::size_t rem = n % kChunkSize;
  if (rem != 0) {
    acc ^= static_cast<uint64_t>(XXH3_64bits(p, rem));
    acc = rotl1(acc);
  }

  return acc;
}
