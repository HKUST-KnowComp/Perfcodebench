#include "interface.h"
#include "xxhash.h"

#include <cstddef>
#include <cstdint>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1024;
}

uint64_t chunked_hash(const std::string& input) {
  uint64_t acc = 0;
  const char* data = input.data();
  const std::size_t size = input.size();
  std::size_t offset = 0;

  // Process full chunks
  for (; offset + kChunkSize <= size; offset += kChunkSize) {
    uint32_t h = XXH32(data + offset, kChunkSize, 0);
    acc ^= static_cast<uint64_t>(h);
    acc = (acc << 1) | (acc >> 63);
  }

  // Process remaining bytes
  if (offset < size) {
    uint32_t h = XXH32(data + offset, size - offset, 0);
    acc ^= static_cast<uint64_t>(h);
    acc = (acc << 1) | (acc >> 63);
  }

  return acc;
}