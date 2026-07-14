#include "interface.h"

#include "xxhash.h"

#include <cstdint>
#include <cstring>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  const char* data = input.data();
  const std::size_t total_size = input.size();
  uint64_t acc = 0;
  
  std::size_t offset = 0;
  while (offset < total_size) {
    const std::size_t remaining = total_size - offset;
    const std::size_t take = (remaining < kChunkSize) ? remaining : kChunkSize;
    acc ^= XXH3_64bits(data + offset, take);
    acc = (acc << 1) | (acc >> 63);
    offset += take;
  }
  
  return acc;
}