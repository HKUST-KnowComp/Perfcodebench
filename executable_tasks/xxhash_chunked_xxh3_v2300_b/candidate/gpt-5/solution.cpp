#include "interface.h"
#include "xxhash.h"

namespace {
constexpr std::size_t kChunkSize = 1u << 10; // 1024 bytes
inline uint64_t rotl1(uint64_t x) {
  return (x << 1) | (x >> 63);
}
}

uint64_t chunked_hash(const std::string& input) {
  const char* p = input.data();
  std::size_t rem = input.size();
  uint64_t acc = 0;

  // Process full chunks
  while (rem >= kChunkSize) {
    acc ^= static_cast<uint64_t>(XXH3_64bits(p, kChunkSize));
    acc = rotl1(acc);
    p += kChunkSize;
    rem -= kChunkSize;
  }

  // Process tail chunk if any
  if (rem) {
    acc ^= static_cast<uint64_t>(XXH3_64bits(p, rem));
    acc = rotl1(acc);
  }

  return acc;
}
