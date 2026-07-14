#include "interface.h"

#include "xxhash.h"

#include <algorithm>
#include <string>
#include <string_view>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
} // namespace

uint64_t chunked_hash(const std::string& input) {
  uint64_t acc = 0;
  std::size_t remaining = input.size();
  std::size_t offset = 0;
  const char* data = input.data();

  while (remaining > 0) {
    const std::size_t take = (remaining < kChunkSize) ? remaining : kChunkSize;
    const uint64_t h = XXH3_64bits(data + offset, take);
    acc ^= h;
    acc = (acc << 1) | (acc >> 63);

    offset += take;
    remaining -= take;
  }
  return acc;
}