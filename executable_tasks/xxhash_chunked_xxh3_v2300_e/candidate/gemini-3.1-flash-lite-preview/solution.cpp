#include "interface.h"
#include "xxhash.h"
#include <cstdint>
#include <algorithm>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  uint64_t acc = 0;
  const char* data = input.data();
  const std::size_t size = input.size();

  for (std::size_t offset = 0; offset < size; offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, size - offset);
    uint64_t h = XXH3_64bits(data + offset, take);
    acc ^= h;
    acc = (acc << 1) | (acc >> 63);
  }
  return acc;
}