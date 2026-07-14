#include "interface.h"
#include "xxhash.h"
#include <cstddef>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  const char* data = input.data();
  std::size_t len = input.size();
  uint64_t acc = 0;
  std::size_t offset = 0;
  while (offset < len) {
    std::size_t take = (offset + kChunkSize <= len) ? kChunkSize : (len - offset);
    acc ^= static_cast<uint64_t>(XXH3_64bits(data + offset, take));
    acc = (acc << 1) | (acc >> 63);
    offset += take;
  }
  return acc;
}