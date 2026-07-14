#include "interface.h"

#include "xxhash.h"

#include <cstdint>
#include <cstring>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  const char* data = input.data();
  const std::size_t size = input.size();
  uint64_t acc = 0;
  
  std::size_t offset = 0;
  
  // Unrolled loop for better ILP
  while (offset + kChunkSize * 2 <= size) {
    // Prefetch next chunks
    __builtin_prefetch(data + offset + kChunkSize * 2, 0, 3);
    
    uint64_t hash1 = XXH64(data + offset, kChunkSize, 0);
    acc ^= hash1;
    acc = (acc << 1) | (acc >> 63);
    
    uint64_t hash2 = XXH64(data + offset + kChunkSize, kChunkSize, 0);
    acc ^= hash2;
    acc = (acc << 1) | (acc >> 63);
    
    offset += kChunkSize * 2;
  }
  
  // Handle remaining chunks
  while (offset < size) {
    const std::size_t take = (offset + kChunkSize <= size) ? kChunkSize : (size - offset);
    const uint64_t hash = XXH64(data + offset, take, 0);
    acc ^= hash;
    acc = (acc << 1) | (acc >> 63);
    offset += kChunkSize;
  }
  
  return acc;
}
