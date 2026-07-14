#include "interface.h"

#include "xxhash.h"

#include <cstdint>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1u << 10;
inline uint64_t rotl1(uint64_t x) { return (x << 1) | (x >> 63); }
}

uint64_t chunked_hash(const std::string& input) {
  const std::size_t size = input.size();
  const char* data = input.data();
  uint64_t acc = 0;

  std::size_t offset = 0;
  const std::size_t fullEnd = size - (size % kChunkSize);
  for (; offset < fullEnd; offset += kChunkSize) {
    const uint64_t h = static_cast<uint64_t>(XXH64(data + offset, kChunkSize, 0));
    acc ^= h;
    acc = rotl1(acc);
  }

  const std::size_t rem = size - offset;
  if (rem) {
    const uint64_t h = static_cast<uint64_t>(XXH64(data + offset, rem, 0));
    acc ^= h;
    acc = rotl1(acc);
  }

  return acc;
}
