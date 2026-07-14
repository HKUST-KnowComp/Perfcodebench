#include "interface.h"

#include "xxhash.h"

#include <cstddef>
#include <cstdint>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1u << 10; // 1024
}

uint64_t chunked_hash(const std::string& input) {
  const char* ptr = input.data();
  std::size_t remaining = input.size();
  uint64_t acc = 0;

  // Process full 1KB chunks
  while (remaining >= kChunkSize) {
    acc ^= static_cast<uint64_t>(XXH3_64bits(ptr, kChunkSize));
    acc = (acc << 1) | (acc >> 63);
    ptr += kChunkSize;
    remaining -= kChunkSize;
  }

  // Process final partial chunk, if any
  if (remaining > 0) {
    acc ^= static_cast<uint64_t>(XXH3_64bits(ptr, remaining));
    acc = (acc << 1) | (acc >> 63);
  }

  return acc;
}
