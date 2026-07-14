#include "interface.h"
#include "xxhash.h"

namespace {
constexpr std::size_t kChunkSize = static_cast<std::size_t>(1) << 10; // 1024
}

uint64_t chunked_hash(const std::string& input) {
  uint64_t acc = 0;
  const char* base = input.data();
  const std::size_t n = input.size();

  const std::size_t full = n & ~(kChunkSize - 1);
  for (std::size_t offset = 0; offset < full; offset += kChunkSize) {
    acc ^= static_cast<uint64_t>(XXH3_64bits(base + offset, kChunkSize));
    acc = (acc << 1) | (acc >> 63);
  }

  const std::size_t rem = n - full;
  if (rem) {
    acc ^= static_cast<uint64_t>(XXH3_64bits(base + full, rem));
    acc = (acc << 1) | (acc >> 63);
  }

  return acc;
}
