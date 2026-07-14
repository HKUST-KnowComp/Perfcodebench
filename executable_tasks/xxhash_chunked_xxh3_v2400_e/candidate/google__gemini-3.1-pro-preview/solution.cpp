#include "interface.h"
#include "xxhash.h"
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  uint64_t acc = 0;
  const char* data = input.data();
  const std::size_t size = input.size();
  std::size_t offset = 0;

  while (offset + kChunkSize <= size) {
    acc ^= static_cast<uint64_t>(XXH3_64bits(data + offset, kChunkSize));
    acc = (acc << 1) | (acc >> 63);
    offset += kChunkSize;
  }

  if (offset < size) {
    acc ^= static_cast<uint64_t>(XXH3_64bits(data + offset, size - offset));
    acc = (acc << 1) | (acc >> 63);
  }

  return acc;
}
