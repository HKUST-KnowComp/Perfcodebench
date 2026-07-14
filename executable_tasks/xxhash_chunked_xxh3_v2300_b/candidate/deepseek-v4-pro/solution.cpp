#include "interface.h"
#include "xxhash.h"
#include <cstddef>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  uint64_t acc = 0;
  const char* data = input.data();
  const std::size_t size = input.size();
  const char* end = data + size;
  while (data + kChunkSize <= end) {
    acc ^= static_cast<uint64_t>(XXH3_64bits(data, kChunkSize));
    acc = (acc << 1) | (acc >> 63);
    data += kChunkSize;
  }
  const std::size_t remaining = end - data;
  if (remaining > 0) {
    acc ^= static_cast<uint64_t>(XXH3_64bits(data, remaining));
    acc = (acc << 1) | (acc >> 63);
  }
  return acc;
}