#include "interface.h"
#include "xxhash.h"

#include <cstddef>
#include <cstdint>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1u << 10; // 1024 bytes
}

uint64_t chunked_hash(const std::string& input) {
  const char* data = input.data();
  const std::size_t size = input.size();

  uint64_t acc = 0;

  // Process full chunks using power-of-two mask to avoid division/modulo.
  const std::size_t mask = kChunkSize - 1;
  const std::size_t full_size = size & ~mask; // largest multiple of kChunkSize <= size

  const char* p = data;
  const char* const end_full = data + full_size;

  while (p < end_full) {
    const XXH64_hash_t h = XXH64(p, kChunkSize, 0);
    acc ^= static_cast<uint64_t>(h);
    acc = (acc << 1) | (acc >> 63);
    p += kChunkSize;
  }

  const std::size_t rem = size - full_size; // equivalent to size & mask
  if (rem) {
    const XXH64_hash_t h = XXH64(p, rem, 0);
    acc ^= static_cast<uint64_t>(h);
    acc = (acc << 1) | (acc >> 63);
  }

  return acc;
}
