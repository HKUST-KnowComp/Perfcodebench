#include "interface.h"
#include "xxhash.h"

namespace {
constexpr std::size_t kChunkSize = 1u << 10; // 1024 bytes

inline uint64_t rotl1(uint64_t x) {
  return (x << 1) | (x >> 63);
}
}

uint64_t chunked_hash(const std::string& input) {
  const char* data = input.data();
  std::size_t n = input.size();
  uint64_t acc = 0;

  std::size_t full_chunks = n / kChunkSize;
  std::size_t offset = 0;

  for (std::size_t i = 0; i < full_chunks; ++i) {
    uint64_t h = static_cast<uint64_t>(XXH64(data + offset, kChunkSize, 0));
    acc ^= h;
    acc = rotl1(acc);
    offset += kChunkSize;
  }

  std::size_t rem = n - offset;
  if (rem != 0) {
    uint64_t h = static_cast<uint64_t>(XXH64(data + offset, rem, 0));
    acc ^= h;
    acc = rotl1(acc);
  }

  return acc;
}
