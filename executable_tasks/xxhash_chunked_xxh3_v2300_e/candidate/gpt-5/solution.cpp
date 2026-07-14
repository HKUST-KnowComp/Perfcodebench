#include "interface.h"
#include "xxhash.h"

namespace {
constexpr std::size_t kChunkSize = 1u << 10; // 1024 bytes
}

uint64_t chunked_hash(const std::string& input) {
  const std::size_t size = input.size();
  const char* ptr = input.data();

  uint64_t acc = 0;

  std::size_t n_full = size / kChunkSize;

  // Process in batches of 4 full chunks to reduce loop overhead
  while (n_full >= 4) {
    uint64_t h0 = XXH3_64bits(ptr, kChunkSize);
    acc ^= h0;
    acc = (acc << 1) | (acc >> 63);

    ptr += kChunkSize;
    uint64_t h1 = XXH3_64bits(ptr, kChunkSize);
    acc ^= h1;
    acc = (acc << 1) | (acc >> 63);

    ptr += kChunkSize;
    uint64_t h2 = XXH3_64bits(ptr, kChunkSize);
    acc ^= h2;
    acc = (acc << 1) | (acc >> 63);

    ptr += kChunkSize;
    uint64_t h3 = XXH3_64bits(ptr, kChunkSize);
    acc ^= h3;
    acc = (acc << 1) | (acc >> 63);

    ptr += kChunkSize;
    n_full -= 4;
  }

  // Process remaining full chunks
  while (n_full > 0) {
    uint64_t h = XXH3_64bits(ptr, kChunkSize);
    acc ^= h;
    acc = (acc << 1) | (acc >> 63);
    ptr += kChunkSize;
    --n_full;
  }

  // Process tail
  const std::size_t tail = size % kChunkSize;
  if (tail != 0) {
    uint64_t h = XXH3_64bits(ptr, tail);
    acc ^= h;
    acc = (acc << 1) | (acc >> 63);
  }

  return acc;
}
