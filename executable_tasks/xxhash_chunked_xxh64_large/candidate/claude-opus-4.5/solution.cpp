#include "interface.h"

#include "xxhash.h"

#include <cstdint>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  const char* data = input.data();
  const std::size_t size = input.size();
  uint64_t acc = 0;
  std::size_t offset = 0;
  
  // Process 4 chunks at a time for better ILP
  const std::size_t stride4 = kChunkSize * 4;
  while (offset + stride4 <= size) {
    // Prefetch ahead
    if (offset + stride4 * 2 <= size) {
      __builtin_prefetch(data + offset + stride4, 0, 0);
    }
    
    uint64_t h0 = XXH64(data + offset, kChunkSize, 0);
    uint64_t h1 = XXH64(data + offset + kChunkSize, kChunkSize, 0);
    uint64_t h2 = XXH64(data + offset + kChunkSize * 2, kChunkSize, 0);
    uint64_t h3 = XXH64(data + offset + kChunkSize * 3, kChunkSize, 0);
    
    acc ^= h0;
    acc = (acc << 1) | (acc >> 63);
    acc ^= h1;
    acc = (acc << 1) | (acc >> 63);
    acc ^= h2;
    acc = (acc << 1) | (acc >> 63);
    acc ^= h3;
    acc = (acc << 1) | (acc >> 63);
    
    offset += stride4;
  }
  
  // Handle remaining chunks
  while (offset < size) {
    const std::size_t take = (offset + kChunkSize <= size) ? kChunkSize : (size - offset);
    acc ^= XXH64(data + offset, take, 0);
    acc = (acc << 1) | (acc >> 63);
    offset += kChunkSize;
  }
  
  return acc;
}