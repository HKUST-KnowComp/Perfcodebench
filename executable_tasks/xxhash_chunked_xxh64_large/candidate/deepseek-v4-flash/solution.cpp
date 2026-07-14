#include "interface.h"
#include "xxhash.h"
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
  uint64_t acc = 0;
  const std::size_t input_size = input.size();
  const char* data = input.data();
  for (std::size_t offset = 0; offset < input_size; offset += kChunkSize) {
    const std::size_t remaining = input_size - offset;
    const std::size_t take = remaining < kChunkSize ? remaining : kChunkSize;
    acc ^= static_cast<uint64_t>(XXH64(data + offset, take, 0));
    acc = (acc << 1) | (acc >> 63);
  }
  return acc;
}
