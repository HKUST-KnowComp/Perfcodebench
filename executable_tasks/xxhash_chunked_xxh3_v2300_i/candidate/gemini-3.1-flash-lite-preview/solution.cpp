#include "interface.h"
#include "xxhash.h"
#include <cstdint>
#include <string>
#include <algorithm>

namespace {
constexpr std::size_t kChunkSize = 1024;
}

uint64_t chunked_hash(const std::string& input) {
  uint64_t acc = 0;
  const char* data = input.data();
  const std::size_t size = input.size();

  for (std::size_t offset = 0; offset < size; offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, size - offset);
    uint64_t h = XXH3_64bits(data + offset, take);
    acc ^= h;
    // Manual rotate left by 1
    acc = (acc << 1) | (acc >> 63);
  }
  return acc;
}