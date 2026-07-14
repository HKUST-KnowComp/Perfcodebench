#include "interface.h"
#include "xxhash.h"

namespace {
constexpr std::size_t kChunkSize = 1u << 10; // 1024 bytes
}

uint64_t chunked_hash(const std::string& input) {
  const char* data = input.data();
  const char* end = data + input.size();

  uint64_t acc = 0;

  const char* p = data;
  // Process full chunks without creating substrings
  while (p + kChunkSize <= end) {
    const uint64_t h = XXH3_64bits(p, kChunkSize);
    acc ^= h;
    acc = (acc << 1) | (acc >> 63);
    p += kChunkSize;
  }

  // Process remaining tail if any
  const std::size_t rem = static_cast<std::size_t>(end - p);
  if (rem) {
    const uint64_t h = XXH3_64bits(p, rem);
    acc ^= h;
    acc = (acc << 1) | (acc >> 63);
  }

  return acc;
}
