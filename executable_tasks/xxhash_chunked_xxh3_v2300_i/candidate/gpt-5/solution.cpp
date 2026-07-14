#include "interface.h"

#include "xxhash.h"

#include <cstdint>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1u << 10; // 1024 bytes
}

uint64_t chunked_hash(const std::string& input) {
  const std::size_t len = input.size();
  if (len == 0) return 0;

  const char* ptr = input.data();
  const char* end = ptr + len;

  uint64_t acc = 0;
  while (ptr < end) {
    const std::size_t remaining = static_cast<std::size_t>(end - ptr);
    const std::size_t take = remaining >= kChunkSize ? kChunkSize : remaining;

    const uint64_t h = XXH3_64bits(ptr, take);
    acc ^= h;
    acc = (acc << 1) | (acc >> 63); // rotate left by 1

    ptr += take;
  }

  return acc;
}
