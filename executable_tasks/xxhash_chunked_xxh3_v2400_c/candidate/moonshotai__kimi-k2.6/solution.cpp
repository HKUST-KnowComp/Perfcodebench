#include "interface.h"

#include "xxhash.h"

#include <cstddef>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  uint64_t acc = 0;
  const std::size_t len = input.size();
  const char* data = input.data();
  std::size_t offset = 0;

  for (; offset + kChunkSize <= len; offset += kChunkSize) {
    acc ^= static_cast<uint64_t>(XXH3_64bits(data + offset, kChunkSize));
    acc = (acc << 1) | (acc >> 63);
  }

  if (offset < len) {
    acc ^= static_cast<uint64_t>(XXH3_64bits(data + offset, len - offset));
    acc = (acc << 1) | (acc >> 63);
  }

  return acc;
}
