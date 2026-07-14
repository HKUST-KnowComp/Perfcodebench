#include "interface.h"
#include "xxhash.h"
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  uint64_t acc = 0;
  std::size_t size = input.size();
  const char* data = input.data();
  for (std::size_t offset = 0; offset < size; offset += kChunkSize) {
    std::size_t take = kChunkSize;
    if (offset + kChunkSize > size) {
      take = size - offset;
    }
    acc ^= XXH3_64bits(data + offset, take);
    acc = (acc << 1) | (acc >> 63);
  }
  return acc;
}
