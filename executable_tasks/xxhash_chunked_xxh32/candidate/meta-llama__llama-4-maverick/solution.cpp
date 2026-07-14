#include "interface.h"
#include "xxhash.h"
#include <algorithm>

namespace {
constexpr std::size_t kChunkSize = 1 << 14;
}

uint64_t chunked_hash(const std::string& input) {
  uint64_t acc = 0;
  const char* data = input.data();
  size_t size = input.size();
  while (size > 0) {
    size_t chunkSize = std::min(kChunkSize, size);
    uint32_t chunkHash = XXH32(data, chunkSize, 0);
    acc ^= static_cast<uint64_t>(chunkHash);
    acc = (acc << 1) | (acc >> 63);
    data += chunkSize;
    size -= chunkSize;
  }
  return acc;
}