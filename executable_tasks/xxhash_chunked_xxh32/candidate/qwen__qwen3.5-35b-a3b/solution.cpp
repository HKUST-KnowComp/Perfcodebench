#include "interface.h"
#include "xxhash.h"

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  uint64_t acc = 0;
  std::size_t len = input.size();
  const char* ptr = input.data();
  const char* end = ptr + len;

  while (ptr < end) {
    std::size_t take = (end - ptr < kChunkSize) ? (end - ptr) : kChunkSize;
    uint32_t hash = XXH32(ptr, take, 0);
    acc ^= static_cast<uint64_t>(hash);
    acc = (acc << 1) | (acc >> 63);
    ptr += take;
  }

  return acc;
}