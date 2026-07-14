#include "interface.h"
#include "xxhash.h"

namespace {
constexpr std::size_t kChunkSize = 1 << 10; // 1024 bytes
}

uint64_t chunked_hash(const std::string& input) {
  const char* p = input.data();
  std::size_t remaining = input.size();
  uint64_t acc = 0;

  // Process full chunks
  while (remaining >= kChunkSize) {
    const uint64_t h = static_cast<uint64_t>(XXH3_64bits(p, kChunkSize));
    acc ^= h;
    acc = (acc << 1) | (acc >> 63);
    p += kChunkSize;
    remaining -= kChunkSize;
  }

  // Process tail
  if (remaining != 0) {
    const uint64_t h = static_cast<uint64_t>(XXH3_64bits(p, remaining));
    acc ^= h;
    acc = (acc << 1) | (acc >> 63);
  }

  return acc;
}
