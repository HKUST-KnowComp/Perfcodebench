#include "interface.h"

#include "xxhash.h"

#include <cstdint>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1u << 10;
}

uint64_t chunked_hash(const std::string& input) {
  const std::size_t size = input.size();
  if (size == 0) return 0;

  const char* ptr = input.data();
  uint64_t acc = 0;

  const std::size_t full_chunks = size / kChunkSize;
  for (std::size_t i = 0; i < full_chunks; ++i) {
    acc ^= static_cast<uint64_t>(XXH3_64bits(ptr, kChunkSize));
    acc = (acc << 1) | (acc >> 63);
    ptr += kChunkSize;
  }

  const std::size_t tail = size - full_chunks * kChunkSize;
  if (tail != 0) {
    acc ^= static_cast<uint64_t>(XXH3_64bits(ptr, tail));
    acc = (acc << 1) | (acc >> 63);
  }

  return acc;
}
