#include "interface.h"

#include "xxhash.h"

#include <cstdint>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  uint64_t acc = 0;
  const char* ptr = input.data();
  std::size_t remaining = input.size();
  
  while (remaining >= kChunkSize) {
    acc ^= XXH3_64bits(ptr, kChunkSize);
    acc = (acc << 1) | (acc >> 63);
    ptr += kChunkSize;
    remaining -= kChunkSize;
  }
  
  if (remaining > 0) {
    acc ^= XXH3_64bits(ptr, remaining);
    acc = (acc << 1) | (acc >> 63);
  }
  
  return acc;
}