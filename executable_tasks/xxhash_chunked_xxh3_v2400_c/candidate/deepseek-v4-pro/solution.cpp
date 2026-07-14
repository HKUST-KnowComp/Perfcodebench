#include "interface.h"
#include "xxhash.h"
#include <algorithm>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  uint64_t acc = 0;
  const char* data = input.data();
  const std::size_t n = input.size();
  for (std::size_t off = 0; off < n; off += kChunkSize) {
    std::size_t take = kChunkSize;
    if (off + take > n) take = n - off;
    acc ^= static_cast<uint64_t>(XXH3_64bits(data + off, take));
    acc = (acc << 1) | (acc >> 63);
  }
  return acc;
}