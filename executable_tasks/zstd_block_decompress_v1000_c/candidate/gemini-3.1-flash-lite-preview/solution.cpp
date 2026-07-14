#include "interface.h"
#include "zstd.h"
#include <vector>
#include <cstdint>

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
  std::vector<uint8_t> restored(decompressed_size);
  
  size_t n = ZSTD_decompress(restored.data(), decompressed_size, compressed.data(), compressed.size());
  
  if (ZSTD_isError(n) || n != decompressed_size) {
    return 0;
  }

  uint64_t hash = 1469598103934665603ULL;
  const uint8_t* data = restored.data();
  for (std::size_t i = 0; i < decompressed_size; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  
  return hash;
}