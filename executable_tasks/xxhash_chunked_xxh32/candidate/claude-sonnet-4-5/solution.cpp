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
  
  // Process two chunks per iteration for better ILP
  while (offset + kChunkSize < size) {
    const std::size_t remaining = size - offset;
    
    if (remaining >= 2 * kChunkSize) {
      // Process two full chunks
      uint32_t hash1 = XXH32(data + offset, kChunkSize, 0);
      uint32_t hash2 = XXH32(data + offset + kChunkSize, kChunkSize, 0);
      
      acc ^= static_cast<uint64_t>(hash1);
      acc = (acc << 1) | (acc >> 63);
      acc ^= static_cast<uint64_t>(hash2);
      acc = (acc << 1) | (acc >> 63);
      
      offset += 2 * kChunkSize;
    } else {
      // Process one full chunk
      uint32_t hash = XXH32(data + offset, kChunkSize, 0);
      acc ^= static_cast<uint64_t>(hash);
      acc = (acc << 1) | (acc >> 63);
      offset += kChunkSize;
    }
  }
  
  // Handle remaining data
  if (offset < size) {
    const std::size_t take = size - offset;
    uint32_t hash = XXH32(data + offset, take, 0);
    acc ^= static_cast<uint64_t>(hash);
    acc = (acc << 1) | (acc >> 63);
  }
  
  return acc;
}