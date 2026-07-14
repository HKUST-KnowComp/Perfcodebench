#include "interface.h"
#include "xxhash.h"
#include <cstdint>
#include <string>
#include <algorithm>

uint64_t chunked_hash(const std::string& input) {
  const char* data = input.data();
  const size_t len = input.size();
  constexpr size_t kChunk = 1024;
  constexpr size_t kStep = 4 * kChunk;
  uint64_t acc = 0;
  size_t offset = 0;

  // Process full chunks in groups of four
  for (; offset + kStep <= len; offset += kStep) {
    uint32_t h0 = XXH32(data + offset,              kChunk, 0);
    uint32_t h1 = XXH32(data + offset + kChunk,     kChunk, 0);
    uint32_t h2 = XXH32(data + offset + 2 * kChunk, kChunk, 0);
    uint32_t h3 = XXH32(data + offset + 3 * kChunk, kChunk, 0);

    acc ^= static_cast<uint64_t>(h0);
    acc = (acc << 1) | (acc >> 63);
    acc ^= static_cast<uint64_t>(h1);
    acc = (acc << 1) | (acc >> 63);
    acc ^= static_cast<uint64_t>(h2);
    acc = (acc << 1) | (acc >> 63);
    acc ^= static_cast<uint64_t>(h3);
    acc = (acc << 1) | (acc >> 63);
  }

  // Process remaining full chunks
  for (; offset + kChunk <= len; offset += kChunk) {
    uint32_t h = XXH32(data + offset, kChunk, 0);
    acc ^= static_cast<uint64_t>(h);
    acc = (acc << 1) | (acc >> 63);
  }

  // Process the last partial chunk
  if (offset < len) {
    uint32_t h = XXH32(data + offset, len - offset, 0);
    acc ^= static_cast<uint64_t>(h);
    acc = (acc << 1) | (acc >> 63);
  }

  return acc;
}
