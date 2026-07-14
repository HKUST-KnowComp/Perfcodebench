#include "interface.h"

#include "xxhash.h"

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
    uint32_t h0 = XXH32(data + offset, kChunkSize, 0);
    uint32_t h1 = XXH32(data + offset + kChunkSize, kChunkSize, 0);
    uint32_t h2 = XXH32(data + offset + kChunkSize * 2, kChunkSize, 0);
    uint32_t h3 = XXH32(data + offset + kChunkSize * 3, kChunkSize, 0);
    
    acc ^= static_cast<uint64_t>(h0);
    acc = (acc << 1) | (acc >> 63);
    acc ^= static_cast<uint64_t>(h1);
    acc = (acc << 1) | (acc >> 63);
    acc ^= static_cast<uint64_t>(h2);
    acc = (acc << 1) | (acc >> 63);
    acc ^= static_cast<uint64_t>(h3);
    acc = (acc << 1) | (acc >> 63);
    
    offset += stride4;
  }
  
  // Handle remaining chunks
  while (offset < size) {
    const std::size_t take = (offset + kChunkSize <= size) ? kChunkSize : (size - offset);
    acc ^= static_cast<uint64_t>(XXH32(data + offset, take, 0));
    acc = (acc << 1) | (acc >> 63);
    offset += kChunkSize;
  }
  
  return acc;
}