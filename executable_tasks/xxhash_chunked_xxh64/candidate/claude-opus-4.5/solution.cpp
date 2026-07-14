#include "interface.h"

#include "xxhash.h"

#include <cstdint>
#include <cstddef>

namespace {
constexpr std::size_t kChunkSize = 1 << 12; // 4KB chunks for better throughput
}

uint64_t chunked_hash(const std::string& input) {
  uint64_t acc = 0;
  const char* data = input.data();
  const std::size_t size = input.size();
  
  std::size_t offset = 0;
  while (offset < size) {
    const std::size_t remaining = size - offset;
    const std::size_t take = (remaining < kChunkSize) ? remaining : kChunkSize;
    acc ^= XXH64(data + offset, take, 0);
    acc = (acc << 1) | (acc >> 63);
    offset += kChunkSize;
  }
  return acc;
}