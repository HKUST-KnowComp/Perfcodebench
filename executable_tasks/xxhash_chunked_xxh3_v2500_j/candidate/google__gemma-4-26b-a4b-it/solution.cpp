#include "interface.h"
#include "xxhash.h"
#include <algorithm>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  uint64_t acc = 0;
  const std::size_t n = input.size();
  const char* data = input.data();

  for (std::size_t offset = 0; offset < n; offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, n - offset);
    // Use the raw pointer and size to avoid std::string::substr heap allocations and copies
    acc ^= static_cast<uint64_t>(XXH3_64bits(data + offset, take));
    // Circular left shift (rotate) by 1
    acc = (acc << 1) | (acc >> 63);
  }
  return acc;
}