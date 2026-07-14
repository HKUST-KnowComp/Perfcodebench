#include "interface.h"

#include "zstd.h"

#include <cstdint>
#include <cstring>
#include <string>

namespace {

__attribute__((always_inline))
inline uint64_t checksum_bytes_fast(const char* data, std::size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  const char* end = data + len;
  
  // Process 8 bytes at a time for better throughput
  while (data + 8 <= end) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[0]));
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[1]));
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[2]));
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[3]));
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[4]));
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[5]));
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[6]));
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[7]));
    hash *= 1099511628211ULL;
    data += 8;
  }
  
  // Handle remaining bytes
  while (data < end) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(*data++));
    hash *= 1099511628211ULL;
  }
  
  return hash;
}

}  // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
  // Use stack buffer for small blocks to avoid heap allocation
  constexpr std::size_t STACK_THRESHOLD = 65536;
  
  if (decompressed_size <= STACK_THRESHOLD) {
    alignas(64) char stack_buf[STACK_THRESHOLD];
    const size_t n = ZSTD_decompress(
        stack_buf, decompressed_size, compressed.data(), compressed.size());
    if (ZSTD_isError(n) || n != decompressed_size) {
      return 0;
    }
    return checksum_bytes_fast(stack_buf, n);
  } else {
    // For larger blocks, use heap allocation
    std::string restored;
    restored.resize(decompressed_size);
    const size_t n = ZSTD_decompress(
        restored.data(), restored.size(), compressed.data(), compressed.size());
    if (ZSTD_isError(n) || n != decompressed_size) {
      return 0;
    }
    return checksum_bytes_fast(restored.data(), n);
  }
}