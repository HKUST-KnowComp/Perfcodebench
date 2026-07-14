#include "interface.h"
#include "xxhash.h"

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  uint64_t acc = 0;
  const char* data = input.data();
  std::size_t remaining = input.size();
  while (remaining >= kChunkSize) {
    acc ^= static_cast<uint64_t>(XXH32(data, kChunkSize, 0));
    acc = (acc << 1) | (acc >> 63);
    data += kChunkSize;
    remaining -= kChunkSize;
  }
  if (remaining > 0) {
    acc ^= static_cast<uint64_t>(XXH32(data, remaining, 0));
    acc = (acc << 1) | (acc >> 63);
  }
  return acc;
}