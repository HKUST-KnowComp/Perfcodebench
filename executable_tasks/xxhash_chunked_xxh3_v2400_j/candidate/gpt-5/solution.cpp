#include "interface.h"
#include "xxhash.h"

namespace {
constexpr std::size_t kChunkSize = 1u << 10;
}

uint64_t chunked_hash(const std::string& input) {
  const char* p = input.data();
  const char* const end = p + input.size();
  uint64_t acc = 0;

  while (p < end) {
    const std::size_t rem = static_cast<std::size_t>(end - p);
    const std::size_t take = rem < kChunkSize ? rem : kChunkSize;
    const uint64_t h = XXH3_64bits(p, take);
    acc ^= h;
    acc = (acc << 1) | (acc >> 63);
    p += take;
  }

  return acc;
}
