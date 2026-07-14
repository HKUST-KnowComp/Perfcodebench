#include "interface.h"

#include "xxhash.h"

#include <cstdint>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1u << 10;

static inline uint64_t rotl1(uint64_t x) {
  return (x << 1) | (x >> 63);
}
}  // namespace

uint64_t chunked_hash(const std::string& input) {
  const std::size_t size = input.size();
  if (size == 0) return 0;

  const char* const data = input.data();
  uint64_t acc = 0;

  const std::size_t full_chunks = size / kChunkSize;
  std::size_t offset = 0;

  for (std::size_t i = 0; i < full_chunks; ++i, offset += kChunkSize) {
    acc ^= static_cast<uint64_t>(XXH3_64bits(data + offset, kChunkSize));
    acc = rotl1(acc);
  }

  const std::size_t tail = size - offset;
  if (tail != 0) {
    acc ^= static_cast<uint64_t>(XXH3_64bits(data + offset, tail));
    acc = rotl1(acc);
  }

  return acc;
}
