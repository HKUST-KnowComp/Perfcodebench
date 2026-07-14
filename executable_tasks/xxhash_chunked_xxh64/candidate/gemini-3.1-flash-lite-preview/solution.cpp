#include "interface.h"
#include "xxhash.h"
#include <algorithm>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  uint64_t acc = 0;
  const char* data = input.data();
  const std::size_t size = input.size();

  for (std::size_t offset = 0; offset < size; offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, size - offset);
    
    // Use XXH64 directly on the buffer slice to avoid string copies
    uint64_t chunk_hash = XXH64(data + offset, take, 0);
    
    // Combine using the same logic as the baseline
    acc ^= chunk_hash;
    acc = (acc << 1) | (acc >> 63);
  }
  return acc;
}