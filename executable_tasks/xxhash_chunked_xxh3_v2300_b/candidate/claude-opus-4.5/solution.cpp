#include "interface.h"

#include "xxhash.h"

#include <cstdint>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  uint64_t acc = 0;
  const char* const data = input.data();
  const std::size_t size = input.size();
  std::size_t offset = 0;
  
  while (offset < size) {
    const std::size_t remaining = size - offset;
    const std::size_t take = (remaining < kChunkSize) ? remaining : kChunkSize;
    acc ^= XXH3_64bits(data + offset, take);
    acc = (acc << 1) | (acc >> 63);
    offset += kChunkSize;
  }
  return acc;
}