#include "interface.h"
#include "zstd.h"
#include <vector>
#include <cstdint>

namespace {

// FNV-1a 64-bit implementation
inline uint64_t checksum_bytes(const char* data, std::size_t size) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t fnv_prime = 1099511628211ULL;
  const unsigned char* p = reinterpret_cast<const unsigned char*>(data);
  for (std::size_t i = 0; i < size; ++i) {
    hash ^= static_cast<uint64_t>(p[i]);
    hash *= fnv_prime;
  }
  return hash;
}

}  // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
  if (decompressed_size == 0) return 1469598103934665603ULL;

  // Use vector to avoid string initialization overhead and unnecessary copies
  std::vector<char> restored(decompressed_size);
  
  const size_t n = ZSTD_decompress(
      restored.data(), 
      restored.size(), 
      compressed.data(), 
      compressed.size());

  if (ZSTD_isError(n) || n != decompressed_size) {
    return 0;
  }

  return checksum_bytes(restored.data(), restored.size());
}