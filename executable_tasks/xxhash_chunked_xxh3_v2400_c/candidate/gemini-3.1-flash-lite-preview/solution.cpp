#include "interface.h"
#include "xxhash.h"
#include <algorithm>
#include <cstdint>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  uint64_t acc = 0;
  const char* data = input.data();
  const std::size_t size = input.size();

  for (std::size_t offset = 0; offset < size; offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, size - offset);
    // Directly hash the slice of the input buffer to avoid allocation
    uint64_t chunk_hash = XXH3_64bits(data + offset, take);
    acc ^= chunk_hash;
    // Rotate left by 1
    acc = (acc << 1) | (acc >> 63);
  }
  return acc;
}