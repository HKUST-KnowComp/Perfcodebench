#include "interface.h"

#include "xxhash.h"

#include <cstdint>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  uint64_t acc = 0;
  const char* data = input.data();
  const std::size_t size = input.size();
  std::size_t offset = 0;
  
  // Process full chunks
  while (offset + kChunkSize <= size) {
    acc ^= XXH3_64bits(data + offset, kChunkSize);
    acc = (acc << 1) | (acc >> 63);
    offset += kChunkSize;
  }
  
  // Process remaining bytes
  if (offset < size) {
    acc ^= XXH3_64bits(data + offset, size - offset);
    acc = (acc << 1) | (acc >> 63);
  }
  
  return acc;
}