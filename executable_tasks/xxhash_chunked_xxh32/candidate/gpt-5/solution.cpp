#include "interface.h"

#include "xxhash.h"

namespace {
constexpr std::size_t kChunkSize = std::size_t{1} << 10; // 1024 bytes
constexpr std::size_t kChunkMask = kChunkSize - 1;
}

uint64_t chunked_hash(const std::string& input) {
  const char* p = input.data();
  const std::size_t len = input.size();
  uint64_t acc = 0;

  // Number of full 1024-byte chunks and remaining tail
  std::size_t full_chunks = len >> 10;            // len / 1024
  std::size_t rem = len & kChunkMask;             // len % 1024

  // Process full chunks, unrolled by 4
  std::size_t blocks4 = full_chunks >> 2;         // groups of 4 chunks
  while (blocks4--) {
    acc ^= static_cast<uint64_t>(XXH32(p, kChunkSize, 0));
    acc = (acc << 1) | (acc >> 63);
    p += kChunkSize;

    acc ^= static_cast<uint64_t>(XXH32(p, kChunkSize, 0));
    acc = (acc << 1) | (acc >> 63);
    p += kChunkSize;

    acc ^= static_cast<uint64_t>(XXH32(p, kChunkSize, 0));
    acc = (acc << 1) | (acc >> 63);
    p += kChunkSize;

    acc ^= static_cast<uint64_t>(XXH32(p, kChunkSize, 0));
    acc = (acc << 1) | (acc >> 63);
    p += kChunkSize;
  }

  // Process remaining full chunks (0..3)
  switch (full_chunks & 3u) {
    case 3:
      acc ^= static_cast<uint64_t>(XXH32(p, kChunkSize, 0));
      acc = (acc << 1) | (acc >> 63);
      p += kChunkSize;
      [[fallthrough]];
    case 2:
      acc ^= static_cast<uint64_t>(XXH32(p, kChunkSize, 0));
      acc = (acc << 1) | (acc >> 63);
      p += kChunkSize;
      [[fallthrough]];
    case 1:
      acc ^= static_cast<uint64_t>(XXH32(p, kChunkSize, 0));
      acc = (acc << 1) | (acc >> 63);
      p += kChunkSize;
      [[fallthrough]];
    case 0:
      break;
  }

  // Process tail if present
  if (rem) {
    acc ^= static_cast<uint64_t>(XXH32(p, rem, 0));
    acc = (acc << 1) | (acc >> 63);
  }

  return acc;
}
