#include "interface.h"
#include "zstd.h"
#include <vector>
#include <cstdint>

namespace {

// Optimized FNV-1a checksum implementation
inline uint64_t checksum_bytes(const uint8_t* data, std::size_t size) {
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < size; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
  if (decompressed_size == 0) return 0;

  // Use a vector to avoid string initialization overhead and unnecessary copies
  std::vector<uint8_t> restored(decompressed_size);
  
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