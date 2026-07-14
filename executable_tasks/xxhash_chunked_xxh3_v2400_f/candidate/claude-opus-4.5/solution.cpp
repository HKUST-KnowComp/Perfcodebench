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
    
    acc ^= static_cast<uint64_t>(XXH3_64bits(data + o0, kChunkSize));
    acc = (acc << 1) | (acc >> 63);
    
    acc ^= static_cast<uint64_t>(XXH3_64bits(data + o1, kChunkSize));
    acc = (acc << 1) | (acc >> 63);
    
    acc ^= static_cast<uint64_t>(XXH3_64bits(data + o2, kChunkSize));
    acc = (acc << 1) | (acc >> 63);
    
    acc ^= static_cast<uint64_t>(XXH3_64bits(data + o3, kChunkSize));
    acc = (acc << 1) | (acc >> 63);
  }
  
  offset = unroll4 * kChunkSize;
  
  // Process remaining full chunks
  for (std::size_t i = unroll4; i < fullChunks; ++i) {
    acc ^= static_cast<uint64_t>(XXH3_64bits(data + offset, kChunkSize));
    acc = (acc << 1) | (acc >> 63);
    offset += kChunkSize;
  }
  
  // Process final partial chunk if any
  if (offset < size) {
    const std::size_t remaining = size - offset;
    acc ^= static_cast<uint64_t>(XXH3_64bits(data + offset, remaining));
    acc = (acc << 1) | (acc >> 63);
  }
  
  return acc;
}