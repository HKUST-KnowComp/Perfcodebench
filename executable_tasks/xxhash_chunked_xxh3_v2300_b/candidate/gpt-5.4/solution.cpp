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

  for (; offset + kChunkSize <= size; offset += kChunkSize) {
    acc ^= static_cast<uint64_t>(XXH3_64bits(data + offset, kChunkSize));
    acc = (acc << 1) | (acc >> 63);
  }

  if (offset < size) {
    const std::size_t tail = size - offset;
    acc ^= static_cast<uint64_t>(XXH3_64bits(data + offset, tail));
    acc = (acc << 1) | (acc >> 63);
  }

  return acc;
}
