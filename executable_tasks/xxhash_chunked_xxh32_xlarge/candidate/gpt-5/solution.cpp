#include "interface.h"

#include "xxhash.h"

#include <cstddef>

namespace {
constexpr std::size_t kChunkSize = 1u << 10; // 1024 bytes
}

uint64_t chunked_hash(const std::string& input) {
  const char* p = input.data();
  std::size_t n = input.size();
  uint64_t acc = 0;

  // Process full 1KB chunks
  while (n >= kChunkSize) {
    acc ^= static_cast<uint64_t>(XXH32(p, kChunkSize, 0));
    acc = (acc << 1) | (acc >> 63);
    p += kChunkSize;
    n -= kChunkSize;
  }

  // Process remaining tail (if any)
  if (n != 0) {
    acc ^= static_cast<uint64_t>(XXH32(p, n, 0));
    acc = (acc << 1) | (acc >> 63);
  }

  return acc;
}
