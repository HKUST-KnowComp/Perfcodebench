#include "interface.h"

#include "xxhash.h"

#include <cstdint>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  uint64_t acc = 0;
  const char* data = input.data();
  const std::size_t size = input.size();
  std::size_t remaining = size;
  
  while (remaining >= kChunkSize) {
    acc ^= XXH3_64bits(data, kChunkSize);
    acc = (acc << 1) | (acc >> 63);
    data += kChunkSize;
    remaining -= kChunkSize;
  }
  
  if (remaining > 0) {
    acc ^= XXH3_64bits(data, remaining);
    acc = (acc << 1) | (acc >> 63);
  }
  
  return acc;
}