#include "interface.h"

#include "xxhash.h"

#include <cstdint>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1u << 10;
}

uint64_t chunked_hash(const std::string& input) {
  const char* const data = input.data();
  const std::size_t size = input.size();

  uint64_t acc = 0;
  std::size_t offset = 0;

  const std::size_t full_end = size & ~(kChunkSize - 1);
  while (offset < full_end) {
    acc ^= static_cast<uint64_t>(XXH3_64bits(data + offset, kChunkSize));
    acc = (acc << 1) | (acc >> 63);
    offset += kChunkSize;
  }

  if (offset < size) {
    const std::size_t tail = size - offset;
    acc ^= static_cast<uint64_t>(XXH3_64bits(data + offset, tail));
    acc = (acc << 1) | (acc >> 63);
  }

  return acc;
}
