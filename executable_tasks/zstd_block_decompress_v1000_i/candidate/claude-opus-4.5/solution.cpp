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
  
  if (__builtin_expect(ZSTD_isError(n) || n != decompressed_size, 0)) {
    return 0;
  }
  
  uint64_t hash = 1469598103934665603ULL;
  const uint8_t* ptr = reinterpret_cast<const uint8_t*>(restored.data());
  const uint8_t* end = ptr + decompressed_size;
  
  // Process 8 bytes at a time
  const uint8_t* end8 = ptr + (decompressed_size & ~7ULL);
  while (ptr < end8) {
    hash ^= static_cast<uint64_t>(ptr[0]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(ptr[1]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(ptr[2]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(ptr[3]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(ptr[4]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(ptr[5]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(ptr[6]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(ptr[7]);
    hash *= 1099511628211ULL;
    ptr += 8;
  }
  
  // Handle remaining bytes
  while (ptr < end) {
    hash ^= static_cast<uint64_t>(*ptr++);
    hash *= 1099511628211ULL;
  }
  
  return hash;
}