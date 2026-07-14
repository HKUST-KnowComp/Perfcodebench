#include "interface.h"
#include "xxhash.h"
#include <algorithm>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  uint64_t acc = 0;
  const std::size_t size = input.size();
  const char* data = input.data();

  for (std::size_t offset = 0; offset < size; offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, size - offset);
    // Use the pointer directly to avoid std::string allocation/copying
    acc ^= static_cast<uint64_t>(XXH3_64bits(data + offset, take));
    // Rotate left 1
    acc = (acc << 1) | (acc >> 63);
  }
  return acc;
}