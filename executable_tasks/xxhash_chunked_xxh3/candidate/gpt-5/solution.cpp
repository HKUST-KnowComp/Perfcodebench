#include "interface.h"
#include "xxhash.h"

#include <cstdint>
#include <cstddef>

namespace {
constexpr std::size_t kChunkSize = std::size_t(1) << 10; // 1024 bytes

inline uint64_t rotl1(uint64_t x) {
  return (x << 1) | (x >> 63);
}
}

uint64_t chunked_hash(const std::string& input) {
  const char* data = input.data();
  const std::size_t n = input.size();

  uint64_t acc = 0;

  // Number of bytes up to the last full chunk boundary (since kChunkSize is a power of two)
  const std::size_t last_full = n & ~(kChunkSize - 1);

  const char* p = data;
  const char* end_full = data + last_full;
  const char* end = data + n;

  // Process full chunks
  while (p < end_full) {
    const XXH64_hash_t h = XXH3_64bits(p, kChunkSize);
    acc ^= static_cast<uint64_t>(h);
    acc = rotl1(acc);
    p += kChunkSize;
  }

  // Process remaining tail, if any
  if (p < end) {
    const std::size_t rem = static_cast<std::size_t>(end - p);
    const XXH64_hash_t h = XXH3_64bits(p, rem);
    acc ^= static_cast<uint64_t>(h);
    acc = rotl1(acc);
  }

  return acc;
}
