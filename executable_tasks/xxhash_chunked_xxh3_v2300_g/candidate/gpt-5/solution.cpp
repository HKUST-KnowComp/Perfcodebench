#include "interface.h"

#include "xxhash.h"

namespace {
constexpr std::size_t kChunkSize = 1 << 10;

inline uint64_t rotl1(uint64_t x) {
  return (x << 1) | (x >> 63);
}
}

uint64_t chunked_hash(const std::string& input) {
  uint64_t acc = 0;
  const std::size_t n = input.size();
  const char* data = input.data();

  const std::size_t fullBytes = (n / kChunkSize) * kChunkSize;
  const char* p = data;
  const char* end = data + fullBytes;

  // Process full chunks with a small unroll for reduced loop overhead
  while (p + 2 * kChunkSize <= end) {
    XXH64_hash_t h0 = XXH3_64bits(p, kChunkSize);
    acc ^= static_cast<uint64_t>(h0);
    acc = rotl1(acc);

    XXH64_hash_t h1 = XXH3_64bits(p + kChunkSize, kChunkSize);
    acc ^= static_cast<uint64_t>(h1);
    acc = rotl1(acc);

    p += 2 * kChunkSize;
  }

  while (p < end) {
    XXH64_hash_t h = XXH3_64bits(p, kChunkSize);
    acc ^= static_cast<uint64_t>(h);
    acc = rotl1(acc);
    p += kChunkSize;
  }

  // Handle remaining bytes (if any)
  const std::size_t rem = n - fullBytes;
  if (rem != 0) {
    XXH64_hash_t h = XXH3_64bits(p, rem);
    acc ^= static_cast<uint64_t>(h);
    acc = rotl1(acc);
  }

  return acc;
}
