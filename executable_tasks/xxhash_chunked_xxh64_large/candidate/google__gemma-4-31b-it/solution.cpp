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
    // Use the pointer directly to avoid std::string::substr allocation
    acc ^= static_cast<uint64_t>(XXH64(data + offset, take, 0));
    acc = (acc << 1) | (acc >> 63);
  }
  return acc;
}