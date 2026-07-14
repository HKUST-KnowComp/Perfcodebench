#include "interface.h"

#include "xxhash.h"

#include <algorithm>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  uint64_t acc = 0;
  const char* ptr = input.data();
  std::size_t remaining = input.size();
  while (remaining != 0) {
    const std::size_t take = std::min(kChunkSize, remaining);
    acc ^= static_cast<uint64_t>(XXH32(ptr, take, 0));
    acc = (acc << 1) | (acc >> 63);
    ptr += take;
    remaining -= take;
  }
  return acc;
}
