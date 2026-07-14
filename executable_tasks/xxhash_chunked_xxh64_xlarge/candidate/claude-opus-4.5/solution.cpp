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
  
  // Process 4 chunks at a time to reduce loop overhead
  const std::size_t fullChunks = size / kChunkSize;
  const std::size_t unroll4 = (fullChunks / 4) * 4;
  
  for (std::size_t i = 0; i < unroll4; i += 4) {
    const std::size_t o0 = i * kChunkSize;
    const std::size_t o1 = o0 + kChunkSize;
    const std::size_t o2 = o1 + kChunkSize;
    const std::size_t o3 = o2 + kChunkSize;
    
    uint64_t h0 = XXH64(data + o0, kChunkSize, 0);
    acc ^= h0;
    acc = (acc << 1) | (acc >> 63);
    
    uint64_t h1 = XXH64(data + o1, kChunkSize, 0);
    acc ^= h1;
    acc = (acc << 1) | (acc >> 63);
    
    uint64_t h2 = XXH64(data + o2, kChunkSize, 0);
    acc ^= h2;
    acc = (acc << 1) | (acc >> 63);
    
    uint64_t h3 = XXH64(data + o3, kChunkSize, 0);
    acc ^= h3;
    acc = (acc << 1) | (acc >> 63);
  }
  
  offset = unroll4 * kChunkSize;
  
  // Process remaining chunks
  while (offset + kChunkSize <= size) {
    acc ^= XXH64(data + offset, kChunkSize, 0);
    acc = (acc << 1) | (acc >> 63);
    offset += kChunkSize;
  }
  
  // Handle final partial chunk if any
  if (offset < size) {
    acc ^= XXH64(data + offset, size - offset, 0);
    acc = (acc << 1) | (acc >> 63);
  }
  
  return acc;
}