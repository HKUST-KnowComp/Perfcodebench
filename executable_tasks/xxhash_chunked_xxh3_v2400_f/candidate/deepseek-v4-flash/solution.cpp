#include "interface.h"
#include "xxhash.h"
#include <cstdint>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  const char* data = input.data();
  std::size_t len = input.size();
  uint64_t acc = 0;
  for (std::size_t offset = 0; offset < len; offset += kChunkSize) {
    std::size_t take = (len - offset < kChunkSize) ? (len - offset) : kChunkSize;
    acc ^= static_cast<uint64_t>(XXH3_64bits(data + offset, take));
    acc = (acc << 1) | (acc >> 63);
  }
  return acc;
}
