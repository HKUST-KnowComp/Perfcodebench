#include "interface.h"

#include "zstd.h"

#include <cstdint>
#include <cstring>
#include <string>

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
  std::string restored;
  restored.resize(decompressed_size);
  
  const size_t n = ZSTD_decompress(
      restored.data(), restored.size(), compressed.data(), compressed.size());
  
  if (ZSTD_isError(n) || n != decompressed_size) {
    return 0;
  }
  
  // FNV-1a checksum with manual unrolling
  uint64_t hash = 1469598103934665603ULL;
  constexpr uint64_t prime = 1099511628211ULL;
  
  const unsigned char* __restrict__ ptr = reinterpret_cast<const unsigned char*>(restored.data());
  const unsigned char* const end = ptr + n;
  const unsigned char* const end8 = ptr + (n & ~static_cast<std::size_t>(7));
  
  // Process 8 bytes at a time
  while (ptr < end8) {
    hash ^= static_cast<uint64_t>(ptr[0]); hash *= prime;
    hash ^= static_cast<uint64_t>(ptr[1]); hash *= prime;
    hash ^= static_cast<uint64_t>(ptr[2]); hash *= prime;
    hash ^= static_cast<uint64_t>(ptr[3]); hash *= prime;
    hash ^= static_cast<uint64_t>(ptr[4]); hash *= prime;
    hash ^= static_cast<uint64_t>(ptr[5]); hash *= prime;
    hash ^= static_cast<uint64_t>(ptr[6]); hash *= prime;
    hash ^= static_cast<uint64_t>(ptr[7]); hash *= prime;
    ptr += 8;
  }
  
  // Handle remaining bytes
  while (ptr < end) {
    hash ^= static_cast<uint64_t>(*ptr++);
    hash *= prime;
  }
  
  return hash;
}