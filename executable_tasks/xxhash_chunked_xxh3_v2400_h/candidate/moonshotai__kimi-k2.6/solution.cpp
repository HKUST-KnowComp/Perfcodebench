#include "interface.h"

#include "xxhash.h"

#include <cstddef>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  const char* const data = input.data();
  const std::size_t total = input.size();
  uint64_t acc = 0;
  for (std::size_t offset = 0; offset < total; offset += kChunkSize) {
    std::size_t take = total - offset;
    if (take > kChunkSize) {
      take = kChunkSize;
    }
    acc ^= static_cast<uint64_t>(XXH3_64bits(data + offset, take));
    acc = (acc << 1) | (acc >> 63);
  }
  return acc;
}
