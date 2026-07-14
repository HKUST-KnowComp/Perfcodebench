#include "interface.h"

#include "xxhash.h"

#include <string>

namespace {
constexpr std::size_t kChunkSize = 1 << 10; // 1024 bytes
}

uint64_t chunked_hash(const std::string& input) {
  const char* data = input.data();
  const std::size_t size = input.size();
  uint64_t acc = 0;

  const std::size_t full = (size / kChunkSize) * kChunkSize;

  // Process full chunks
  for (std::size_t offset = 0; offset < full; offset += kChunkSize) {
    uint64_t h = static_cast<uint64_t>(XXH3_64bits(data + offset, kChunkSize));
    acc ^= h;
    acc = (acc << 1) | (acc >> 63);
  }

  // Process tail, if any
  const std::size_t tail = size - full;
  if (tail) {
    uint64_t h = static_cast<uint64_t>(XXH3_64bits(data + full, tail));
    acc ^= h;
    acc = (acc << 1) | (acc >> 63);
  }

  return acc;
}
