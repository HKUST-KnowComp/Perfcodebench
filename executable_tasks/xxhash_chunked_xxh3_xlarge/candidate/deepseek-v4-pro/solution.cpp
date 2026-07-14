#include "interface.h"

#include "xxhash.h"

#include <cstddef>
#include <cstdint>
#include <string>

uint64_t chunked_hash(const std::string& input) {
  static constexpr std::size_t kChunkSize = 1 << 10;
  const std::size_t total = input.size();
  if (total == 0) {
    return 0;
  }

  const std::size_t full_chunks = total / kChunkSize;
  const std::size_t remainder = total % kChunkSize;
  uint64_t acc = 0;
  const char* data = input.data();

  for (std::size_t i = 0; i < full_chunks; ++i) {
    acc ^= static_cast<uint64_t>(XXH3_64bits(data + i * kChunkSize, kChunkSize));
    acc = (acc << 1) | (acc >> 63);
  }

  if (remainder != 0) {
    acc ^= static_cast<uint64_t>(XXH3_64bits(data + full_chunks * kChunkSize, remainder));
    acc = (acc << 1) | (acc >> 63);
  }

  return acc;
}