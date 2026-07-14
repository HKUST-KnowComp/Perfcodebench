#include "interface.h"

#include "zstd.h"

#include <cstdint>
#include <cstring>

namespace {

uint64_t checksum_bytes(const char* data, std::size_t size) {
  uint64_t hash = 1469598103934665603ULL;
  const unsigned char* ptr = reinterpret_cast<const unsigned char*>(data);
  std::size_t i = 0;
  
  // Process 8 bytes at a time with manual unrolling
  const std::size_t unroll = 8;
  const std::size_t limit = size - (size % unroll);
  
  for (; i < limit; i += unroll) {
    uint64_t h0 = hash;
    uint64_t h1 = hash;
    uint64_t h2 = hash;
    uint64_t h3 = hash;
    uint64_t h4 = hash;
    uint64_t h5 = hash;
    uint64_t h6 = hash;
    uint64_t h7 = hash;
    
    h0 ^= static_cast<uint64_t>(ptr[i]);
    h1 ^= static_cast<uint64_t>(ptr[i+1]);
    h2 ^= static_cast<uint64_t>(ptr[i+2]);
    h3 ^= static_cast<uint64_t>(ptr[i+3]);
    h4 ^= static_cast<uint64_t>(ptr[i+4]);
    h5 ^= static_cast<uint64_t>(ptr[i+5]);
    h6 ^= static_cast<uint64_t>(ptr[i+6]);
    h7 ^= static_cast<uint64_t>(ptr[i+7]);
    
    h0 *= 1099511628211ULL;
    h1 *= 1099511628211ULL;
    h2 *= 1099511628211ULL;
    h3 *= 1099511628211ULL;
    h4 *= 1099511628211ULL;
    h5 *= 1099511628211ULL;
    h6 *= 1099511628211ULL;
    h7 *= 1099511628211ULL;
    
    hash = h0 ^ h1 ^ h2 ^ h3 ^ h4 ^ h5 ^ h6 ^ h7;
  }
  
  // Handle remaining bytes
  for (; i < size; ++i) {
    hash ^= static_cast<uint64_t>(ptr[i]);
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
  
  return checksum_bytes(restored.data(), n);
}
