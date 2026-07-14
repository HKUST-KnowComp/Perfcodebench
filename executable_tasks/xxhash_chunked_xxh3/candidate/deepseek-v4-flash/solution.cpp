#include "interface.h"
#include "xxhash.h"
#include <cstdint>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  const char* data = input.data();
  std::size_t remaining = input.size();
  uint64_t acc = 0;
  while (remaining > 0) {
    const std::size_t take = (remaining < kChunkSize) ? remaining : kChunkSize;
    acc ^= static_cast<uint64_t>(XXH3_64bits(data, take));
    acc = (acc << 1) | (acc >> 63);
    data += take;
    remaining -= take;
  }
  return acc;
}
