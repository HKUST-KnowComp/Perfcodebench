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
  const std::size_t total_size = input.size();
  std::size_t offset = 0;

  while (offset < total_size) {
    const std::size_t remaining = total_size - offset;
    const std::size_t take = (remaining < kChunkSize) ? remaining : kChunkSize;
    
    // Use the pointer directly to avoid std::string::substr allocation/copy
    acc ^= XXH3_64bits(data + offset, take);
    
    // Circular left shift (rotate) by 1
    acc = (acc << 1) | (acc >> 63);
    
    offset += take;
  }

  return acc;
}