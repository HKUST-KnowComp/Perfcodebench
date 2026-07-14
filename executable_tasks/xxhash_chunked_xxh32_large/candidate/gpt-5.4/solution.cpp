#include "interface.h"

#include "xxhash.h"

namespace {
constexpr std::size_t kChunkSize = 1u << 10;
}

uint64_t chunked_hash(const std::string& input) {
  const char* data = input.data();
  const std::size_t size = input.size();

  uint64_t acc = 0;
  std::size_t offset = 0;

  const std::size_t full_end = size & ~(kChunkSize - 1);
  for (; offset < full_end; offset += kChunkSize) {
    acc ^= static_cast<uint64_t>(XXH32(data + offset, kChunkSize, 0));
    acc = (acc << 1) | (acc >> 63);
  }

  if (offset < size) {
    acc ^= static_cast<uint64_t>(XXH32(data + offset, size - offset, 0));
    acc = (acc << 1) | (acc >> 63);
  }

  return acc;
}
