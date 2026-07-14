#include "interface.h"

#include "zstd.h"

#include <cstdint>
#include <cstring>
#include <string>

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
  // Allocate raw buffer - avoid string overhead
  char* restored = new char[decompressed_size];
  
  // Decompress directly from input - no copy needed
  const size_t n = ZSTD_decompress(
      restored, decompressed_size, compressed.data(), compressed.size());
  
  if (ZSTD_isError(n) || n != decompressed_size) {
    delete[] restored;
    return 0;
  }
  
  // FNV-1a checksum with unrolled loop
  uint64_t hash = 1469598103934665603ULL;
  constexpr uint64_t prime = 1099511628211ULL;
  
  const unsigned char* data = reinterpret_cast<const unsigned char*>(restored);
  size_t len = decompressed_size;
  
  // Process 8 bytes at a time
  size_t i = 0;
  const size_t unroll_end = len & ~size_t(7);
  
  for (; i < unroll_end; i += 8) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(data[i + 1]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(data[i + 2]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(data[i + 3]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(data[i + 4]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(data[i + 5]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(data[i + 6]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(data[i + 7]);
    hash *= prime;
  }
  
  // Handle remaining bytes
  for (; i < len; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= prime;
  }
  
  delete[] restored;
  return hash;
}