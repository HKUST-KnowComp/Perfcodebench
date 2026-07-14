#include "interface.h"

#include "zstd.h"

#include <cstdint>
#include <cstring>
#include <string>

namespace {

__attribute__((always_inline))
inline uint64_t checksum_bytes_fast(const char* __restrict__ data, std::size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  constexpr uint64_t PRIME = 1099511628211ULL;
  
  const unsigned char* __restrict__ p = reinterpret_cast<const unsigned char*>(data);
  const unsigned char* const end = p + len;
  
  // Prefetch first cache lines
  if (len >= 64) {
    __builtin_prefetch(p + 64, 0, 3);
    __builtin_prefetch(p + 128, 0, 3);
  }
  
  // Process 8 bytes at a time for better ILP
  const unsigned char* const end8 = p + (len & ~7ULL);
  std::size_t prefetch_offset = 256;
  
  while (p < end8) {
    // Prefetch ahead
    if (p + prefetch_offset < end) {
      __builtin_prefetch(p + prefetch_offset, 0, 3);
    }
    
    // Unroll 8 iterations
    hash ^= static_cast<uint64_t>(p[0]);
    hash *= PRIME;
    hash ^= static_cast<uint64_t>(p[1]);
    hash *= PRIME;
    hash ^= static_cast<uint64_t>(p[2]);
    hash *= PRIME;
    hash ^= static_cast<uint64_t>(p[3]);
    hash *= PRIME;
    hash ^= static_cast<uint64_t>(p[4]);
    hash *= PRIME;
    hash ^= static_cast<uint64_t>(p[5]);
    hash *= PRIME;
    hash ^= static_cast<uint64_t>(p[6]);
    hash *= PRIME;
    hash ^= static_cast<uint64_t>(p[7]);
    hash *= PRIME;
    
    p += 8;
  }
  
  // Handle remaining bytes
  while (p < end) {
    hash ^= static_cast<uint64_t>(*p++);
    hash *= PRIME;
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
  
  return checksum_bytes_fast(restored.data(), n);
}