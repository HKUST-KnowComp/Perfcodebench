#include "interface.h"

#include "xxhash.h"

namespace {
constexpr std::size_t kChunkSize = 1 << 10; // 1024 bytes

inline uint64_t rotl1(uint64_t x) {
  return (x << 1) | (x >> 63);
}
}  // namespace

uint64_t chunked_hash(const std::string& input) {
  const std::size_t size = input.size();
  if (size == 0) return 0ULL;

  const char* p = input.data();
  uint64_t acc = 0ULL;

  const std::size_t full_chunks = size / kChunkSize;
  const std::size_t full_bytes = full_chunks * kChunkSize;
  const char* const end_full = p + full_bytes;

  // Process all full chunks (fixed size: kChunkSize)
  while (p < end_full) {
    const uint64_t h = static_cast<uint64_t>(XXH3_64bits(p, kChunkSize));
    acc ^= h;
    acc = rotl1(acc);
    p += kChunkSize;
  }

  // Process the final partial chunk if present
  const std::size_t rem = size - full_bytes;
  if (rem) {
    const uint64_t h = static_cast<uint64_t>(XXH3_64bits(p, rem));
    acc ^= h;
    acc = rotl1(acc);
  }

  return acc;
}
