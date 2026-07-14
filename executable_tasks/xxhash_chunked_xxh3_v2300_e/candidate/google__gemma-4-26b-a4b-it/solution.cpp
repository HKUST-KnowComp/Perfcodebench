#include "interface.h"
#include "xxhash.h"
#include <string>
#include <algorithm>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  uint64_t acc = 0;
  const char* data = input.data();
  const std::size_t size = input.size();
  
  for (std::size_t offset = 0; offset < size; offset += kChunkSize) {
    const std::size_t remaining = size - offset;
    const std::size_t current_chunk_size = (remaining < kChunkSize) ? remaining : kChunkSize;
    
    // Use the pointer directly to avoid std::string::substr allocations/copies
    acc ^= XXH3_64bits(data + offset, current_chunk_size);
    
    // Circular left shift (rotate left by 1)
    acc = (acc << 1) | (acc >> 63);
  }
  
  return acc;
}