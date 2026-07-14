#include "interface.h"

#include "xxhash.h"

#include <algorithm>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  uint64_t acc = 0;
  const char* data = input.data();
  const std::size_t total_size = input.size();
  for (std::size_t offset = 0; offset < total_size; offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, total_size - offset);
    acc ^= static_cast<uint64_t>(XXH3_64bits(data + offset, take));
    acc = (acc << 1) | (acc >> 63);
  }
  return acc;
}
