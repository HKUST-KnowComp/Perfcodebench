#include "interface.h"

#include "xxhash.h"

#include <cstdint>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  uint64_t acc = 0;
  const char* data = input.data();
  const std::size_t len = input.size();
  const char* const end = data + len;
  
  while (data < end) {
    const std::size_t remaining = static_cast<std::size_t>(end - data);
    const std::size_t take = remaining < kChunkSize ? remaining : kChunkSize;
    acc ^= XXH3_64bits(data, take);
    acc = (acc << 1) | (acc >> 63);
    data += take;
  }
  return acc;
}
