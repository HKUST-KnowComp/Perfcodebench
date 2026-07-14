#include "interface.h"

#include "xxhash.h"

#include <algorithm>
#include <string>
#include <cstddef>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

inline uint64_t rotl1(uint64_t x) {
    return (x << 1) | (x >> 63);
}

uint64_t chunked_hash(const std::string& input) {
  uint64_t acc = 0;
  const char* data = input.data();
  std::size_t size = input.size();
  for (std::size_t offset = 0; offset < size; offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, size - offset);
    acc ^= static_cast<uint64_t>(XXH3_64bits(data + offset, take));
    acc = rotl1(acc);
  }
  return acc;
}
