#include "interface.h"
#include "xxhash.h"
#include <algorithm>
#include <cstdint>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;

inline uint64_t rotl64(uint64_t x, int r) {
  return (x << r) | (x >> (64 - r));
}
}

uint64_t chunked_hash(const std::string& input) {
  uint64_t acc = 0;
  const char* data = input.data();
  const std::size_t size = input.size();

  for (std::size_t offset = 0; offset < size; offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, size - offset);
    uint64_t h = XXH3_64bits(data + offset, take);
    acc ^= h;
    acc = rotl64(acc, 1);
  }
  return acc;
}