#include "interface.h"

#include "zstd.h"

#include <cstring>
#include <string>

namespace {

uint64_t checksum_bytes(const char* data, std::size_t size) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  
  std::size_t i = 0;
  
  // Process 8 bytes at a time when possible
  const std::size_t aligned_end = size & ~7ULL;
  for (; i < aligned_end; i += 8) {
    uint64_t chunk;
    std::memcpy(&chunk, data + i, 8);
    
    hash ^= (chunk & 0xFF);
    hash *= prime;
    hash ^= ((chunk >> 8) & 0xFF);
    hash *= prime;
    hash ^= ((chunk >> 16) & 0xFF);
    hash *= prime;
    hash ^= ((chunk >> 24) & 0xFF);
    hash *= prime;
    hash ^= ((chunk >> 32) & 0xFF);
    hash *= prime;
    hash ^= ((chunk >> 40) & 0xFF);
    hash *= prime;
    hash ^= ((chunk >> 48) & 0xFF);
    hash *= prime;
    hash ^= ((chunk >> 56) & 0xFF);
    hash *= prime;
  }
  
  // Handle remaining bytes
  for (; i < size; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= prime;
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
