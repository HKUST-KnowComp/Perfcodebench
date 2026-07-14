#include "interface.h"

#include "xxhash.h"

#include <cstdint>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  uint64_t acc = 0;
  const char* ptr = input.data();
  const char* const end = ptr + input.size();
  
  while (ptr < end) {
    const std::size_t remaining = static_cast<std::size_t>(end - ptr);
    const std::size_t take = remaining < kChunkSize ? remaining : kChunkSize;
    acc ^= XXH3_64bits(ptr, take);
    acc = (acc << 1) | (acc >> 63);
    ptr += take;
  }
  return acc;
}