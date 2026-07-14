#include "interface.h"

#include "xxhash.h"

#include <cstdint>
#include <cstddef>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  uint64_t acc = 0;
  const char* data = input.data();
  const std::size_t size = input.size();
  std::size_t offset = 0;

  // Process 4 chunks at a time when possible
  const std::size_t chunk4 = kChunkSize * 4;
  while (offset + chunk4 <= size) {
    uint64_t h0 = XXH3_64bits(data + offset, kChunkSize);
    uint64_t h1 = XXH3_64bits(data + offset + kChunkSize, kChunkSize);
    uint64_t h2 = XXH3_64bits(data + offset + kChunkSize * 2, kChunkSize);
    uint64_t h3 = XXH3_64bits(data + offset + kChunkSize * 3, kChunkSize);

    acc ^= h0;
    acc = (acc << 1) | (acc >> 63);
    acc ^= h1;
    acc = (acc << 1) | (acc >> 63);
    acc ^= h2;
    acc = (acc << 1) | (acc >> 63);
    acc ^= h3;
    acc = (acc << 1) | (acc >> 63);

    offset += chunk4;
  }

  // Handle remaining chunks
  while (offset < size) {
    const std::size_t take = (offset + kChunkSize <= size) ? kChunkSize : (size - offset);
    acc ^= XXH3_64bits(data + offset, take);
    acc = (acc << 1) | (acc >> 63);
    offset += take;
  }

  return acc;
}