#include "interface.h"
#include "xxhash.h"

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  const std::size_t input_size = input.size();
  const char* const data = input.data();
  uint64_t acc = 0;
  for (std::size_t offset = 0; offset < input_size; offset += kChunkSize) {
    const std::size_t take = (offset + kChunkSize <= input_size) ? kChunkSize : (input_size - offset);
    acc ^= static_cast<uint64_t>(XXH3_64bits(data + offset, take));
    acc = (acc << 1) | (acc >> 63);
  }
  return acc;
}
