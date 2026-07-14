#include "interface.h"
#include "xxhash.h"

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  uint64_t acc = 0;
  for (std::size_t offset = 0; offset < input.size(); offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, input.size() - offset);
    acc ^= XXH64(input.data() + offset, take, 0);
    acc = (acc << 1) | (acc >> 63);
  }
  return acc;
}