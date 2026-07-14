#include "interface.h"

#include "xxhash.h"

#include <string>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  uint64_t acc = 0;
  const char* ptr = input.data();
  const char* end = ptr + input.size();

  while (ptr < end) {
    std::size_t chunk_len = static_cast<std::size_t>(end - ptr);
    if (chunk_len > kChunkSize) {
      chunk_len = kChunkSize;
    }
    acc ^= static_cast<uint64_t>(XXH3_64bits(ptr, chunk_len));
    acc = (acc << 1) | (acc >> 63);
    ptr += chunk_len;
  }
  return acc;
}