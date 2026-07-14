#include "interface.h"

#include "xxhash.h"

#include <string>
#include <cstddef>
#include <cstdint>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  uint64_t acc = 0;
  const char* data = input.data();
  const std::size_t size = input.size();

  std::size_t offset = 0;
  const std::size_t full_end = size & ~(kChunkSize - 1); // largest multiple of kChunkSize <= size

  // Process full chunks
  while (offset < full_end) {
    acc ^= static_cast<uint64_t>(XXH3_64bits(data + offset, kChunkSize));
    acc = (acc << 1) | (acc >> 63);
    offset += kChunkSize;
  }

  // Process remaining bytes
  if (offset < size) {
    const std::size_t rem = size - offset;
    acc ^= static_cast<uint64_t>(XXH3_64bits(data + offset, rem));
    acc = (acc << 1) | (acc >> 63);
  }

  return acc;
}
