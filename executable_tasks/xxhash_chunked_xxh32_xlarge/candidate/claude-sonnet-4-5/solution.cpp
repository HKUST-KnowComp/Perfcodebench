#include "interface.h"

#include "xxhash.h"

#include <algorithm>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  const std::size_t size = input.size();
  const char* data = input.data();
  uint64_t acc = 0;
  
  std::size_t offset = 0;
  
  // Process full chunks
  const std::size_t full_chunks = size / kChunkSize;
  for (std::size_t i = 0; i < full_chunks; ++i, offset += kChunkSize) {
    const uint64_t hash = XXH32(data + offset, kChunkSize, 0);
    acc ^= hash;
    acc = (acc << 1) | (acc >> 63);
  }
  
  // Process remaining bytes
  if (offset < size) {
    const std::size_t remaining = size - offset;
    const uint64_t hash = XXH32(data + offset, remaining, 0);
    acc ^= hash;
    acc = (acc << 1) | (acc >> 63);
  }
  
  return acc;
}