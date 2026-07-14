#include "interface.h"

#include "zstd.h"

#include <string>

namespace {

uint64_t checksum_bytes(const char* data, std::size_t size) {
  uint64_t hash = 1469598103934665603ULL;
  const unsigned char* ptr = reinterpret_cast<const unsigned char*>(data);
  const unsigned char* end = ptr + size;
  
  // Process 8 bytes at a time with separate hash chains
  const unsigned char* end8 = ptr + (size / 8) * 8;
  uint64_t h0 = hash, h1 = hash, h2 = hash, h3 = hash;
  uint64_t h4 = hash, h5 = hash, h6 = hash, h7 = hash;
  
  while (ptr < end8) {
    h0 ^= ptr[0]; h0 *= 1099511628211ULL;
    h1 ^= ptr[1]; h1 *= 1099511628211ULL;
    h2 ^= ptr[2]; h2 *= 1099511628211ULL;
    h3 ^= ptr[3]; h3 *= 1099511628211ULL;
    h4 ^= ptr[4]; h4 *= 1099511628211ULL;
    h5 ^= ptr[5]; h5 *= 1099511628211ULL;
    h6 ^= ptr[6]; h6 *= 1099511628211ULL;
    h7 ^= ptr[7]; h7 *= 1099511628211ULL;
    ptr += 8;
  }
  
  // Combine hash chains
  hash = h0 ^ h1 ^ h2 ^ h3 ^ h4 ^ h5 ^ h6 ^ h7;
  
  // Process remaining bytes
  while (ptr < end) {
    hash ^= *ptr++;
    hash *= 1099511628211ULL;
  }
  
  return hash;
}

}  // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
  std::string restored;
  restored.resize(decompressed_size);
  const size_t n = ZSTD_decompress(
      restored.data(), restored.size(), compressed.data(), compressed.size());
  if (ZSTD_isError(n) || n != decompressed_size) {
    return 0;
  }
  return checksum_bytes(restored.data(), decompressed_size);
}
