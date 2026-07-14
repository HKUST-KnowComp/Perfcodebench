#include "interface.h"

#include "xxhash.h"

#include <algorithm>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  uint64_t acc = 0;
  for (std::size_t offset = 0; offset < input.size(); offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, input.size() - offset);
    const std::string chunk = input.substr(offset, take);
    acc ^= static_cast<uint64_t>(XXH64(chunk.data(), chunk.size(), 0));
    acc = (acc << 1) | (acc >> 63);
  }
  return acc;
}
