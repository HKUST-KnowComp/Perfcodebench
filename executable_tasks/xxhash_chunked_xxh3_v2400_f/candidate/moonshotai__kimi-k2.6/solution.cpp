#include "interface.h"

#include "xxhash.h"

#include <cstddef>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  uint64_t acc = 0;
  const char* data = input.data();
  const std::size_t size = input.size();

  const std::size_t full_chunks = size / kChunkSize;
  const std::size_t remainder = size % kChunkSize;

  std::size_t offset = 0;
  for (std::size_t i = 0; i < full_chunks; ++i) {
    acc ^= static_cast<uint64_t>(XXH3_64bits(data + offset, kChunkSize));
    acc = (acc << 1) | (acc >> 63);
    offset += kChunkSize;
  }

  if (remainder != 0) {
    acc ^= static_cast<uint64_t>(XXH3_64bits(data + offset, remainder));
    acc = (acc << 1) | (acc >> 63);
  }

  return acc;
}
